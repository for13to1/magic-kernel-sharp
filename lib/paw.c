#include "paw.h"

typedef union {
  paw p;
  struct {
    ushort m : 8;
    ushort e : 7;
    ushort n : 1;
  } a;
} C_;

typedef union {
  double d;
  struct {
    ulong m : 52;
    ulong e : 11;
    ulong n : 1;
  } a;
} D_;


paw paw_make(double d) {
  bool n;
  ushort e;
  ushort m;
  ulong dm;
  ulong r;
  C_ c;
  D_ dc;

  dc.d = d;

  if ((e = (ushort)dc.a.e) == 0) {
    return paw_zero();
  }

  dm = dc.a.m;
  n = dc.a.n == 1;

  if (e == 0x7ff) {
    if (dm == 0) {
      return n ? paw_negative_infinity() : paw_positive_infinity();
    } else {
      return paw_nan();
    }
  }

  m = (ushort)(dm >> 44);
  r = dm & 0xfffffffffff;

  if (r > 0x80000000000 || (r == 0x80000000000 && (m & 0x1) == 0x1)) {
    if (++m == 0x100) {
      e++;
    }
  }

  if (e > 1086) {
    return n ? paw_negative_infinity() : paw_positive_infinity();
  }

  if (e < 961) {
    return paw_zero();
  }

  c.a.e = e - 960;
  c.a.n = n;
  c.a.m = m;

  return c.p;
}

paw paw_from_parts(bool negative, byte exponent, byte mantissa) {
  C_ c;

  c.a.n = negative;
  c.a.e = exponent;
  c.a.m = mantissa;

  return c.p;
}

void paw_print(paw this, Printer *p) {
  C_ c;

  c.p = this;

  sprint(p, "paw");
  rprint(p, "Negative: %s", bool_string(c.a.n));
  rprint(p, "Mantissa: 0x%x", c.a.m);
  rprint(p, "Exponent: 0x%x", c.a.e);
  rprint(p, "Exponent - 0x3f = %d", (short)c.a.e - 0x3f);
  rprint(p, "Value: %.4g", double_from_paw(this));
  fprint(p);
}

bool paw_equals(paw this, paw other) {
  return other.u == this.u;
}

bool paw_gt(paw this, paw other) {
  return double_from_paw(this) > double_from_paw(other);
}

bool paw_lt(paw this, paw other) {
  return double_from_paw(this) < double_from_paw(other);
}

bool paw_ge(paw this, paw other) {
  return double_from_paw(this) >= double_from_paw(other);
}

bool paw_le(paw this, paw other) {
  return double_from_paw(this) <= double_from_paw(other);
}

double double_from_paw(paw p) {
  C_ c;
  D_ d;

  if (paw_is_zero(p)) {
    return 0;
  }

  if (paw_is_positive_infinity(p)) {
    return double_positive_infinity();
  }

  if (paw_is_negative_infinity(p)) {
    return double_negative_infinity();
  }

  if (paw_is_nan(p)) {
    return double_nan();
  }

  c.p = p;
  d.a.n = c.a.n;
  d.a.e = c.a.e + 960;
  d.a.m = ((ulong)c.a.m) << 44;

  return d.d;
}

paw paw_zero(void) {
  paw p = {0};

  return p;
}

paw paw_one(void) {
  return paw_make(1);
}

paw paw_half(void) {
  return paw_make(0.5);
}

paw paw_positive_infinity(void) {
  C_ c;

  c.a.n = 0;
  c.a.e = 0x7f;
  c.a.m = 0;

  return c.p;
}

paw paw_negative_infinity(void) {
  C_ c;

  c.a.n = 1;
  c.a.e = 0x7f;
  c.a.m = 0x0;

  return c.p;
}

paw paw_nan(void) {
  C_ c;

  c.a.n = 0;
  c.a.e = 0x7f;
  c.a.m = 0x1;

  return c.p;
}

bool paw_is_zero(paw p) {
  return p.u == 0;
}

bool paw_is_positive_infinity(paw p) {
  C_ c;

  c.p = p;

  return c.a.e == 0x7f && c.a.m == 0x0 && c.a.n == 0;
}

bool paw_is_negative_infinity(paw p) {
  C_ c;

  c.p = p;

  return c.a.e == 0x7f && c.a.m == 0x0 && c.a.n == 1;
}

bool paw_is_nan(paw p) {
  C_ c;

  c.p = p;

  return c.a.e == 0x7f && c.a.m != 0x0;
}

bool paw_is_infinite_or_nan(paw p) {
  C_ c;

  c.p = p;

  return c.a.e == 0x7f;
}

paw paw_negate(paw p) {
  if (paw_is_zero(p) || paw_is_nan(p)) {
    return p;
  }

  p.u ^= 0x8000;

  return p;
}

paw paw_add(paw a, paw b) {
  return paw_make(double_from_paw(a) + double_from_paw(b));
}

paw paw_subtract(paw a, paw b) {
  return paw_make(double_from_paw(a) - double_from_paw(b));
}

paw paw_multiply(paw a, paw b) {
  return paw_make(double_from_paw(a) * double_from_paw(b));
}

paw paw_divide(paw a, paw b) {
  return paw_make(double_from_paw(a) / double_from_paw(b));
}

paw paw_bound(paw x, paw min, paw max) {
  return paw_make(bound(
        double_from_paw(x),
        double_from_paw(min),
        double_from_paw(max)));
}

paw paw_min(paw a, paw b) {
  return paw_make(minimum(double_from_paw(a), double_from_paw(b)));
}

paw paw_max(paw a, paw b) {
  return paw_make(maximum(double_from_paw(a), double_from_paw(b)));
}

sbyte paw_compare(paw *a, paw *b, void *x) {
  double da;
  double db;

  da = double_from_paw(*a);
  db = double_from_paw(*b);

  return da < db ? -1 : da > db ? +1 : 0;
}
