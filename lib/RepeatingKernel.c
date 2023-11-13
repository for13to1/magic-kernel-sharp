#include "RepeatingKernel.h"
#include <limits.h>

#define MIN_ 100000

typedef struct {
  RepeatingKernel *this;
  double (*read)(long in, void *x);
  void (*write)(ulong out, double v, void *x);
  void *x;
} Pass_;

static KernelPossible identity_possible_(ulong out, void *x);

static double identity_weight_(ulong out, long in, void *x);

static RepeatingKernel *new_(RepeatingKernel **p_this, ulong n_in, ulong n_out);

static void apply_(Parallel *par);


void repeating_kernel_new(
    RepeatingKernel **p_this,
    ulong n_in,
    ulong n_out,
    KernelPossible (*possible)(ulong out, void *x),
    double (*weight)(ulong out, long in, void *x),
    void *x) {
  long first;
  long in;
  long last;
  long min;
  long max;
  ulong n;
  ulong num_elements;
  ulong out;
  double scale;
  double sum_w;
  double w;
  double *weights;
  double *_ws = null;
  KernelPossible possible_in;
  RepeatingKernel *this;

  if (possible == null) {
    throw("'possible' function pointer cannot be null");
  }

  if (weight == null) {
    throw("'weight' function pointer cannot be null");
  }

  this = new_(p_this, n_in, n_out);

  num_elements = 0;

  for (out = 0; out < n_out; out++) {
    possible_in = possible(out, x);
    first = possible_in.first;
    last = possible_in.last;

    if (last < first) {
      throw("Last (%ld) cannot be less than first (%ld) for out = %lu",
          last,
          first,
          out);
    }

    n = last - first + 1;
    new(&_ws, n);
    _ws -= first;

    sum_w = 0;
    min = LONG_MAX;
    max = LONG_MIN;

    for (in = first; in <= last; in++) {
      w = _ws[in] = weight(out, in, x);
      sum_w += w;

      if (w != 0) {
        min = minimum(min, in);
        max = maximum(max, in);
      }
    }

    if (sum_w <= 0) {
      throw("Sum of weights (%.16g) for out = %lu was not positive, "
          "which is not supported",
          sum_w,
          out);
    }

    scale = 1 / sum_w;
    this->_firsts[out] = min;
    this->_lasts[out] = max;
    n = max - min + 1;
    new(&this->_weights[out], n);
    this->_weights[out] -= min;
    weights = this->_weights[out];

    for (in = min; in <= max; in++) {
      weights[in] = _ws[in] * scale;
    }

    num_elements += n;
    _ws += first;
    deletev(&_ws);
  }

  this->numElements = num_elements;
  this->minParallel = MIN_ * this->nOut / this->numElements;
}

void repeating_kernel_new_identity(RepeatingKernel **p_this, ulong n) {
  repeating_kernel_new(
      p_this,
      n,
      n,
      identity_possible_,
      identity_weight_,
      null);
}

void repeating_kernel_delete(RepeatingKernel **p_this) {
  ulong out;
  RepeatingKernel *this;

  if ((this = *p_this) == null) {
    return;
  }

  for (out = 0; out < this->nOut; out++) {
    this->_weights[out] += this->_firsts[out];
    deletev(&this->_weights[out]);
  }

  deletev(&this->_weights);
  deletev(&this->_firsts);
  deletev(&this->_lasts);

  deleteo(p_this);
}

void repeating_kernel_print(RepeatingKernel *this, Printer *p) {
  ulong out;
  long first;
  long in;
  long last;
  double *weights;

  sprint(p, "RepeatingKernel");
  rprint(p, "n_in: %lu", this->nIn);
  rprint(p, "n_out: %lu", this->nOut);
  rprint(p, "num_elements: %lu", this->numElements);

  for (out = 0; out < this->nOut; out++) {
    sprint(p, "out = %lu", out);
    first = this->_firsts[out];
    last = this->_lasts[out];
    weights = this->_weights[out];

    for (in = first; in <= last; in++) {
      rprint(p, "in = %ld: w = %.16g", in, weights[in]);
    }

    fprint(p);
  }

  fprint(p);
}

double repeating_kernel_get(RepeatingKernel *this, ulong out, long in) {
  long first;
  long last;

  if (out >= this->nOut) {
    throw("out (%lu) must be less than n_out (%lu)", out, this->nOut);
  }

  first = this->_firsts[out];
  last = this->_lasts[out];

  if (in < first || in > last) {
    throw("in (%ld) must be in [%ld, %ld]", in, first, last);
  }

  return this->_weights[out][in];
}

void repeating_kernel_apply(
    RepeatingKernel *this,
    ulong n_out,
    double (*read)(long in, void *x),
    void (*write)(ulong out, double v, void *x),
    void *x) {
  Pass_ pass;

  pass.read = read;
  pass.write = write;
  pass.x = x;
  pass.this = this;

  parallel_for(n_out, this->minParallel, apply_, &pass);
}


static RepeatingKernel *new_(
    RepeatingKernel **p_this,
    ulong n_in,
    ulong n_out) {
  RepeatingKernel *this;

  if (n_in == 0) {
    throw("n_in cannot be zero");
  }

  if (n_out == 0) {
    throw("n_out cannot be zero");
  }

  this = newo(p_this);

  this->nIn = n_in;
  this->nOut = n_out;

  new(&this->_firsts, n_out);
  new(&this->_lasts, n_out);
  new(&this->_weights, n_out);

  return this;
}

static void apply_(Parallel *par) {
  ulong chunk;
  ulong n_in;
  ulong n_out;
  ulong out;
  ulong rel_out;
  long base_in;
  long first;
  long in;
  long last;
  long rel_first;
  long rel_in;
  long rel_last;
  double sum;
  double *weights;
  Pass_ *pass;
  RepeatingKernel *this;

  pass = par->x;

  this = pass->this;
  n_in = this->nIn;
  n_out = this->nOut;

  for (out = par->start; out < par->end; out++) {
    chunk = out / n_out;
    rel_out = out % n_out;

    base_in = (long)(chunk * n_in);

    rel_first = this->_firsts[rel_out];
    rel_last = this->_lasts[rel_out];
    weights = this->_weights[rel_out];

    first = rel_first + base_in;
    last = rel_last + base_in;

    sum = 0;

    for (in = first; in <= last; in++) {
      rel_in = in - base_in;
      sum += weights[rel_in] * pass->read(in, pass->x);
    }

    pass->write(out, sum, pass->x);
  }
}

static KernelPossible identity_possible_(ulong out, void *x) {
  KernelPossible possible;

  possible.first = possible.last = (long)out;

  return possible;
}

static double identity_weight_(ulong out, long in, void *x) {
  return 1;
}
