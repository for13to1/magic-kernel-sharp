#include "IeeeDouble.h"
#include <math.h>

typedef union {
  double d;
  struct {
    ulong m : 52;
    ulong e : 11;
    ulong n : 1;
  } a;
} Converter_;


IeeeDouble ieee_double_make(double d) {
  Converter_ c;
  IeeeDouble this;

  c.d = d;
  this.mantissa = c.a.m;
  this.exponent = c.a.e;
  this.negative = c.a.n;

  return this;
}

void ieee_double_print(IeeeDouble this, Printer *p) {
  sprint(p, "IeeeDouble");
  rprint(p, "Negative: %s", bool_string(this.negative));
  rprint(p, "Mantissa: 0x%lx", this.mantissa);
  rprint(p, "Exponent: 0x%lx", this.exponent);
  rprint(p, "Exponent - 0x3ff = %d", (short)this.exponent - 0x3ff);
  fprint(p);
}

double double_from_ieee_double(IeeeDouble i) {
  Converter_ c;

  c.a.m = i.mantissa;
  c.a.e = i.exponent;
  c.a.n = i.negative;

  return c.d;
}

double double_from_parts(bool negative, ushort exponent, ulong mantissa) {
  IeeeDouble i;

  i.negative = negative;
  i.exponent = exponent;
  i.mantissa = mantissa;

  return double_from_ieee_double(i);
}

byte ieee_double_significant_bits(IeeeDouble i) {
  if (ieee_double_is_subnormal(i)) {
    throw("Cannot get number of significant bits for subnormal double %.16g",
        double_from_ieee_double(i));
  }

  if (ieee_double_is_infinite_or_nan(i)) {
    throw("Cannot get number of significant bits for infinite or NaN "
        "double %.16g",
        double_from_ieee_double(i));
  }

  if (i.exponent == 0) {
    return 0;
  }

  return ulong_bit_width(ulong_reverse_bits(i.mantissa << 12)) + 1;
}


byte double_significant_bits(double d) {
  return ieee_double_significant_bits(ieee_double_make(d));
}

ulong ulong_from_ieee_double(IeeeDouble i) {
  return (ulong)i.negative << 63 | (ulong)i.exponent << 52 | i.mantissa;
}

ulong ulong_from_double(double d) {
  return ulong_from_ieee_double(ieee_double_make(d));
}

IeeeDouble ieee_double_from_ulong(ulong u) {
  IeeeDouble i;

  i.negative = u >> 63;
  i.exponent = u >> 52;
  i.mantissa = u;

  return i;
}

double double_from_ulong(ulong u) {
  return double_from_ieee_double(ieee_double_from_ulong(u));
}

IeeeDouble ieee_double_negative_zero(void) {
  IeeeDouble i;

  i.negative = 1;
  i.exponent = 0x0;
  i.mantissa = 0x0;

  return i;
}

double double_negative_zero(void) {
  return double_from_ieee_double(ieee_double_negative_zero());
}

IeeeDouble ieee_double_positive_infinity(void) {
  IeeeDouble i;

  i.negative = 0;
  i.exponent = 0x7ff;
  i.mantissa = 0x0;

  return i;
}

double double_positive_infinity(void) {
  return double_from_ieee_double(ieee_double_positive_infinity());
}

IeeeDouble ieee_double_negative_infinity(void) {
  IeeeDouble i;

  i.negative = 1;
  i.exponent = 0x7ff;
  i.mantissa = 0x0;

  return i;
}

double double_negative_infinity(void) {
  return double_from_ieee_double(ieee_double_negative_infinity());
}

IeeeDouble ieee_double_nan(void) {
  IeeeDouble i;

  i.negative = 0;
  i.exponent = 0x7ff;
  i.mantissa = 0x1;

  return i;
}

double double_nan(void) {
  return double_from_ieee_double(ieee_double_nan());
}

IeeeDouble ieee_double_subnormal(void) {
  IeeeDouble i;

  i.negative = 0;
  i.exponent = 0x0;
  i.mantissa = 0x1;

  return i;
}

double double_subnormal(void) {
  return double_from_ieee_double(ieee_double_subnormal());
}

bool ieee_double_is_negative_zero(IeeeDouble i) {
  return i.exponent == 0x0 && i.negative == 1 && i.mantissa == 0x0;
}

bool double_is_negative_zero(double d) {
  return ieee_double_is_negative_zero(ieee_double_make(d));
}

bool ieee_double_is_positive_infinity(IeeeDouble i) {
  return i.exponent == 0x7ff && i.negative == 0 && i.mantissa == 0x0;
}

bool double_is_positive_infinity(double d) {
  return ieee_double_is_positive_infinity(ieee_double_make(d));
}

bool ieee_double_is_negative_infinity(IeeeDouble i) {
  return i.exponent == 0x7ff && i.negative == 1 && i.mantissa == 0x0;
}

bool double_is_negative_infinity(double d) {
  return ieee_double_is_negative_infinity(ieee_double_make(d));
}

bool ieee_double_is_nan(IeeeDouble i) {
  return i.exponent == 0x7ff && i.mantissa != 0x0;
}

bool double_is_nan(double d) {
  return ieee_double_is_nan(ieee_double_make(d));
}

bool ieee_double_is_subnormal(IeeeDouble i) {
  return i.exponent == 0x0 && i.mantissa != 0x0;
}

bool double_is_subnormal(double d) {
  return ieee_double_is_subnormal(ieee_double_make(d));
}

bool ieee_double_is_negative_zero_or_subnormal(IeeeDouble i) {
  return i.exponent == 0x0 && (i.negative == 1 || i.mantissa != 0x0);
}

bool double_is_negative_zero_or_subnormal(double d) {
  return ieee_double_is_negative_zero_or_subnormal(ieee_double_make(d));
}

bool ieee_double_is_infinite_or_nan(IeeeDouble i) {
  return i.exponent == 0x7ff;
}

bool double_is_infinite_or_nan(double d) {
  return ieee_double_is_infinite_or_nan(ieee_double_make(d));
}
