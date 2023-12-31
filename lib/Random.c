#include "Random.h"

#define N_ 624
#define M_ 397

void random_new(Random **p_this) {
  random_new_seed(p_this, unixtime_current_us());
}

void random_new_seed(Random **p_this, uint seed) {
  uint *m;
  int i;
  Random *this;

  this = newo(p_this);

  new(&this->_m, N_);
  m = this->_m;

  m[0] = seed;

  for (i = 1; i < N_; i++) {
    m[i] = 1812433253 * (m[i - 1] ^ (m[i - 1] >> 30)) + i;
  }

  this->i = i;
}

void random_new_offset(Random **p_this, uint offset) {
  uint jump;
  Random *_random = null;

  random_new(&_random);
  jump = (uint)(19680707 + random_modulo(_random, 19660924));
  random_new_seed(p_this, unixtime_current_us() + jump);

  random_delete(&_random);
}

void random_delete(Random **p_this) {
  Random *this;

  if ((this = *p_this) == null) {
    return;
  }

  deletev(&this->_m);

  deleteo(p_this);
}

ulong random_modulo(Random *this, ulong modulus) {
  ulong u;
  ulong limit;

  if (modulus == 0) {
    throw("Modulus cannot be zero");
  }

  if (modulus > 0x0fffffff) {
    if (modulus != this->lastModulus) {
      limit = this->lastLimit = 0xffffffffffffffff / modulus * modulus;
      this->lastModulus = modulus;
    } else {
      limit = this->lastLimit;
    }

    do {
      u = random_ulong(this);
    } while (u >= limit);
  } else {
    if (modulus != this->lastModulus) {
      limit = this->lastLimit = 0xffffffff / modulus * modulus;
      this->lastModulus = modulus;
    } else {
      limit = this->lastLimit;
    }

    do {
      u = random_uint(this);
    } while (u >= limit);
  }

  return u % modulus;
}

double random_double(Random *this) {
  return double_from_parts(
      random_modulo(this, 2),
      1 + random_modulo(this, 0x7fe),
      random_modulo(this, 0x10000000000000));
}

struct paw random_paw(Random *this) {
  return paw_make(double_from_parts(
      random_modulo(this, 2),
      1 + random_modulo(this, 0x7e),
      random_modulo(this, 0x10000)));
}

double random_double_unit_cc(Random *this) {
  static const double k__ = 1.0 / (double)0xffffffffffffffff;

  return random_ulong(this) * k__;
}

double random_double_unit_co(Random *this) {
  double d;

  for (;;) {
    d = random_double_unit_cc(this);

    if (d < 1) {
      return d;
    }
  }
}

double random_double_unit_oc(Random *this) {
  double d;

  for (;;) {
    d = random_double_unit_cc(this);

    if (d > 0) {
      return d;
    }
  }
}

double random_double_unit_oo(Random *this) {
  double d;

  for (;;) {
    d = random_double_unit_cc(this);

    if (d > 0 && d < 1) {
      return d;
    }
  }
}

bool random_bool(Random *this) {
  return random_modulo(this, 2) == 1;
}

uint random_uint(Random *this) {
  static const uint g__[] = {0, 0x9908b0df};

  int k;
  uint y;
  uint *m;

  m = this->_m;

  if (this->i == N_) {
    for (k = 0; k < N_ - M_; k++) {
      y = (m[k] & 0x80000000) | (m[k + 1] & 0x7fffffff);
      m[k] = m[k + M_] ^ (y >> 1) ^ g__[y & 1];
    }

    for (; k < N_ - 1; k++) {
      y = (m[k] & 0x80000000) | (m[k + 1] & 0x7fffffff);
      m[k] = m[k + (M_ - N_)] ^ (y >> 1) ^ g__[y & 1];
    }

    y = (m[N_ - 1] & 0x80000000) | (m[0] & 0x7fffffff);
    m[N_ - 1] = m[M_ - 1] ^ (y >> 1) ^ g__[y & 1];

    this->i = 0;
  }

  y = m[this->i++];

  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680;
  y ^= (y << 15) & 0xefc60000;
  y ^= (y >> 18);

  return y;
}

ulong random_ulong(Random *this) {
  return ((ulong)random_uint(this) << 32) | random_uint(this);
}

long random_long(Random *this) {
  union {
    ulong u;
    long l;
  } c;

  c.u = random_ulong(this);
  return c.l;
}
