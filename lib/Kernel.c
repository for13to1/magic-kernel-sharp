#include "Kernel.h"
#include <limits.h>
#include <math.h>

#define MIN_ 100000

typedef struct {
  Kernel *this;
  double (*read)(ulong in, void *x);
  void (*write)(ulong out, double v, void *x);
  void *x;
} Pass_;

static ulong new_(Kernel **p_this, ulong n_in, ulong n_out, bool periodic);

static void apply_(Parallel *par);

static ulong bound_(long in, ulong n_in, bool periodic);

static KernelPossible identity_possible_(ulong out, void *x);

static double identity_weight_(ulong out, long in, void *x);

static ulong create_(ulong **pins,
    double **pweights,
    bool normalize,
    ulong n_in,
    ulong out,
    double *ws,
    ulong min,
    ulong max);


void kernel_new(
    Kernel **p_this,
    ulong n_in,
    ulong n_out,
    bool periodic,
    bool normalize,
    KernelPossible (*possible)(ulong out, void *x),
    double (*weight)(ulong out, long in, void *x),
    void *x) {
  ulong bounded;
  ulong min;
  ulong max;
  ulong n;
  ulong out;
  ulong num_elements;
  long first;
  long in;
  long last;
  double *_ws = null;
  KernelPossible possible_in;
  Kernel *this;

  new_(p_this, n_in, n_out, periodic);
  this = *p_this;

  if (possible == null) {
    throw("'possible' function pointer cannot be null");
  }

  if (weight == null) {
    throw("'weight' function pointer cannot be null");
  }

  num_elements = 0;

  for (out = 0; out < n_out; out++) {
    new(&_ws, n_in);
    possible_in = possible(out, x);
    first = possible_in.first;
    last = possible_in.last;

    if (last < first) {
      throw("last (%ld) cannot be less than first (%ld) for out = %lu",
          last,
          first,
          out);
    }

    min = ULONG_MAX;
    max = 0;

    for (in = first; in <= last; in++) {
      bounded = bound_(in, n_in, periodic);
      min = minimum(min, bounded);
      max = maximum(max, bounded);
      _ws[bounded] += weight(out, in, x);
    }

    n = create_(&this->_ins[out],
        &this->_weights[out],
        normalize,
        n_in,
        out,
        _ws,
        min,
        max);

    this->_ns[out] = n;
    num_elements += n;
    deletev(&_ws);
  }

  this->numElements = num_elements;
  this->minParallel = MIN_ * this->nOut / this->numElements;
}

void kernel_new_compose(Kernel **p_this, Kernel *k1, Kernel *k2) {
  ulong first_1;
  ulong a_1;
  ulong a_2;
  ulong in;
  ulong join;
  ulong last_1;
  ulong n_in;
  ulong n_join;
  ulong n_out;
  ulong num_elements;
  ulong out;
  ulong n;
  ulong n_1;
  ulong n_2;
  ulong *ins_1;
  ulong *ins_2;
  ulong *ns_1;
  ulong *ns_2;
  ulong **all_ins_1;
  ulong **all_ins_2;
  double w;
  double *_ws = null;
  double **weights_1;
  double **weights_2;
  Kernel *this;

  if (k2->nIn != (n_join = k1->nOut)) {
    throw("n_in (%lu) for the second kernel must equal "
        "n_out (%lu) of the first kernel",
        k2->nIn,
        k1->nOut);
  }

  if (k1->periodic != k2->periodic) {
    throw("Periodic flag for first kernel (%s) must match that "
        "of the second (%s)",
        bool_string(k1->periodic),
        bool_string(k2->periodic));
  }

  n_out = k2->nOut;
  n_in = k1->nIn;

  new_(p_this, n_in, n_out, k1->periodic);
  this = *p_this;

  num_elements = 0;
  ns_1 = k1->_ns;
  ns_2 = k2->_ns;
  all_ins_1 = k1->_ins;
  all_ins_2 = k2->_ins;
  weights_1 = k1->_weights;
  weights_2 = k2->_weights;

  for (out = 0; out < n_out; out++) {
    new(&_ws, n_in);
    n_2 = ns_2[out];
    ins_2 = all_ins_2[out];

    for (in = 0; in < n_in; in++) {
      for (a_2 = 0, w = 0; a_2 < n_2; a_2++) {
        join = ins_2[a_2];

        n_1 = ns_1[join];
        ins_1 = all_ins_1[join];
        first_1 = ins_1[0];
        last_1 = ins_1[n_1 - 1];

        if (in < first_1 || in > last_1) {
          continue;
        }

        for (a_1 = 0; a_1 < n_1; a_1++) {
          if (ins_1[a_1] == in) {
            w += weights_2[out][a_2] * weights_1[join][a_1];
            break;
          }
        }
      }

      _ws[in] = w;
    }

    n = create_(&this->_ins[out],
        &this->_weights[out],
        true,
        n_in,
        out,
        _ws,
        0,
        n_in - 1);

    this->_ns[out] = n;
    num_elements += n;
    deletev(&_ws);
  }

  this->numElements = num_elements;
  this->minParallel = MIN_ * this->nOut / this->numElements;
}

void kernel_new_identity(Kernel **p_this, ulong n) {
  kernel_new(
      p_this,
      n,
      n,
      false,
      true,
      identity_possible_,
      identity_weight_,
      null);
}

void kernel_delete(Kernel **p_this) {
  ulong out;
  Kernel *this;

  if ((this = *p_this) == null) {
    return;
  }

  for (out = 0; out < this->nOut; out++) {
    deletev(&this->_weights[out]);
    deletev(&this->_ins[out]);
  }

  deletev(&this->_weights);
  deletev(&this->_ins);
  deletev(&this->_ns);

  deleteo(p_this);
}

void kernel_print(Kernel *this, Printer *p) {
  ulong a;
  ulong nn;
  ulong out;
  ulong *ins;
  double *weights;

  sprint(p, "Kernel");
  rprint(p, "n_in: %lu", this->nIn);
  rprint(p, "n_out: %lu", this->nOut);
  rprint(p, "num_elements: %lu", this->numElements);
  rprint(p, "min_parallel: %lu", this->minParallel);
  rprint(p, "periodic: %s", bool_string(this->periodic));

  for (out = 0; out < this->nOut; out++) {
    sprint(p, "out = %lu", out);
    nn = this->_ns[out];
    ins = this->_ins[out];
    weights = this->_weights[out];

    for (a = 0; a < nn; a++) {
      rprint(p, "in = %lu: w = %.16g", ins[a],  weights[a]);
    }

    fprint(p);
  }

  fprint(p);
}

void kernel_apply(
    Kernel *this,
    double (*read)(ulong in, void *x),
    void (*write)(ulong out, double v, void *x),
    void *x) {
  Pass_ pass;

  pass.read = read;
  pass.write = write;
  pass.x = x;
  pass.this = this;

  parallel_for(this->nOut, this->minParallel, apply_, &pass);
}

bool kernel_equals(Kernel *this, Kernel *other, double tolerance) {
  ulong in;
  ulong n_in;
  ulong n_out;
  ulong out;
  double w_this;
  double w_other;

  if ((n_in = this->nIn) != other->nIn) {
    return false;
  }

  if ((n_out = this->nOut) != other->nOut) {
    return false;
  }

  for (out = 0; out < n_out; out++) {
    for (in = 0; in < n_in; in++) {
      w_this = kernel_get(this, out, in);
      w_other = kernel_get(other, out, in);

      if (fabs(w_other - w_this) > tolerance) {
        return false;
      }
    }
  }

  return true;
}

double kernel_resize_out_from_in(double in, double k) {
  return k * (in + 0.5) - 0.5;
}

double kernel_resize_in_from_out(double out, double k) {
  return (out + 0.5) / k - 0.5;
}

double kernel_get(Kernel *this, ulong out, ulong in) {
  ulong a;
  ulong n;
  ulong *ins;

  if (in >= this->nIn) {
    throw("in (%lu) must be less than n_in (%lu)", in, this->nIn);
  }

  if (out >= this->nOut) {
    throw("out (%lu) must be less than n_out (%lu)", out, this->nOut);
  }

  n = this->_ns[out];
  ins = this->_ins[out];

  if (in < ins[0] || in > ins[n - 1]) {
    return 0;
  }

  if (!bsearchx_try_find_index(&in, ins, n, ulong_compare, null, &a)) {
    return 0;
  }

  return this->_weights[out][a];
}

static ulong new_(Kernel **p_this, ulong n_in, ulong n_out, bool periodic) {
  Kernel *this;

  if (n_in == 0) {
    throw("n_in cannot be zero");
  }
  if (n_out == 0) {
    throw("n_out cannot be zero");
  }

  this = newo(p_this);

  this->nIn = n_in;
  this->nOut = n_out;
  this->periodic = periodic;

  new(&this->_ns, n_out);
  new(&this->_ins, n_out);
  new(&this->_weights, n_out);

  return 0;
}

static KernelPossible identity_possible_(ulong out, void *x) {
  KernelPossible possible;

  possible.first = possible.last = (long)out;

  return possible;
}

static double identity_weight_(ulong out, long in, void *x) {
  return 1;
}

static ulong bound_(long in, ulong n_in, bool periodic) {
  if (in >= 0 && in < n_in) {
    return in;
  }
  if (periodic) {
    return modulo(in, n_in);
  }
  if (in < 0) {
    return 0;
  }
  return n_in - 1;
}

static ulong create_(ulong **pins,
    double **pweights,
    bool normalize,
    ulong n_in,
    ulong out,
    double *ws,
    ulong min,
    ulong max) {
  ulong a;
  ulong in;
  ulong n;
  ulong *ins;
  double normalization;
  double sum;
  double w;
  double *weights;

  for (in = min, n = 0, sum = 0; in <= max; in++) {
    if ((w = ws[in]) != 0) {
      n++;
      sum += w;
    }
  }

  if (n == 0) {
    throw("Did not have any nonzero weights for out = %lu", out);
  }

  if (normalize && sum <= 0) {
    throw("Sum of weights (%.16g) for out = %lu was not positive, which is not "
        "supported",
        sum,
        out);
  }

  normalization = normalize ? 1 / sum : 1;

  new(pins, n);
  ins = *pins;

  new(pweights, n);
  weights = *pweights;

  for (in = min, a = 0; in <= max; in++) {
    if ((w = ws[in]) != 0) {
      ins[a] = in;
      weights[a++] = w * normalization;
    }
  }

  return n;
}

static void apply_(Parallel *par) {
  ulong a;
  ulong n;
  ulong out;
  ulong *ins;
  double sum;
  double *weights;
  Pass_ *pass;
  Kernel *this;

  pass = par->x;

  this = pass->this;

  for (out = par->start; out < par->end; out++) {
    n = this->_ns[out];
    ins = this->_ins[out];
    weights = this->_weights[out];

    sum = 0;

    for (a = 0, sum = 0; a < n; a++) {
      sum += weights[a] * pass->read(ins[a], pass->x);
    }

    pass->write(out, sum, pass->x);
  }
}
