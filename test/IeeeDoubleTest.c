#include "IeeeDoubleTest.h"
#include <float.h>
#include <math.h>

START

TEST(IeeeDouble, Size) {
  EXPECT_ULEQ(8, sizeof(IeeeDouble));
}

TEST(IeeeDouble, Make) {
  IeeeDouble i;

  i = ieee_double_make(0);
  EXPECT_ULEQ(0, i.negative);
  EXPECT_ULEQ(0x0, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);

  i = ieee_double_make(1);
  EXPECT_ULEQ(0, i.negative);
  EXPECT_ULEQ(0x3ff, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);

  i = ieee_double_make(-1);
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x3ff, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);

  i = ieee_double_make(2);
  EXPECT_ULEQ(0, i.negative);
  EXPECT_ULEQ(0x400, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);

  i = ieee_double_make(-2);
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x400, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);

  i = ieee_double_make(0.5);
  EXPECT_ULEQ(0, i.negative);
  EXPECT_ULEQ(0x3fe, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);

  i = ieee_double_make(-0.5);
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x3fe, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);

  i = ieee_double_make(3);
  EXPECT_ULEQ(0, i.negative);
  EXPECT_ULEQ(0x400, i.exponent);
  EXPECT_ULEQ(0x8000000000000, i.mantissa);

  i = ieee_double_make(-0.75);
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x3fe, i.exponent);
  EXPECT_ULEQ(0x8000000000000, i.mantissa);

  i = ieee_double_make(1.0 / 3);
  EXPECT_ULEQ(0, i.negative);
  EXPECT_ULEQ(0x3fd, i.exponent);
  EXPECT_ULEQ(0x5555555555555, i.mantissa);

  i = ieee_double_make(-1.234567890123456e-123);
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x266, i.exponent);
  EXPECT_ULEQ(0xa1da47cb534db, i.mantissa);

  i = ieee_double_make(-DBL_MAX);
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x7fe, i.exponent);
  EXPECT_ULEQ(0xfffffffffffff, i.mantissa);

  i = ieee_double_make(-DBL_MIN);
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x1, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);

  i = ieee_double_make(DBL_MIN);
  EXPECT_ULEQ(0, i.negative);
  EXPECT_ULEQ(0x1, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);

  i = ieee_double_make(DBL_MAX);
  EXPECT_ULEQ(0, i.negative);
  EXPECT_ULEQ(0x7fe, i.exponent);
  EXPECT_ULEQ(0xfffffffffffff, i.mantissa);

  i = ieee_double_make(double_subnormal());
  EXPECT_ULEQ(0, i.negative);
  EXPECT_ULEQ(0x0, i.exponent);
  EXPECT_ULEQ(0x1, i.mantissa);

  i = ieee_double_make(1.0 / (-1.0 / 0));
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x0, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);

  i = ieee_double_make(double_positive_infinity());
  EXPECT_ULEQ(0, i.negative);
  EXPECT_ULEQ(0x7ff, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);

  i = ieee_double_make(double_negative_infinity());
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x7ff, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);

  i = ieee_double_make(double_nan());
  EXPECT_ULEQ(0x7ff, i.exponent);
  EXPECT_TRUE(i.mantissa != 0x0);

  /* Computational way to create negative zero.
   */
  i = ieee_double_make(1.0 / (-1.0 / 0));
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x0, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);

  /* Computational way to create positive infinity.
   */
  i = ieee_double_make(1.0 / 0);
  EXPECT_ULEQ(0, i.negative);
  EXPECT_ULEQ(0x7ff, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);

  /* Computational way to create negative infinity.
   */
  i = ieee_double_make(-1.0 / 0);
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x7ff, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);

  /* Computational way to create not a number.
   */
  i = ieee_double_make(sqrt(-1));
  EXPECT_ULEQ(0x7ff, i.exponent);
  EXPECT_TRUE(i.mantissa != 0x0);
}

TEST(IeeeDouble, Print) {
  char *_s = null;
  IeeeDouble i;

  i = ieee_double_make(-0.75);
  aprint(&_s, 0, ieee_double, i);
  EXPECT_STREQ(
      "[IeeeDouble]\n. Negative: true\n. Mantissa: 0x 8 0000 0000 0000\n"
      ". Exponent: 0x3fe\n. Exponent - 0x3ff = -1\n[/IeeeDouble]\n",
      _s);
  string_delete(&_s);

  i = ieee_double_make(-1.234567890123456e-123);
  aprint(&_s, 0, ieee_double, i);
  EXPECT_STREQ(
      "[IeeeDouble]\n. Negative: true\n. Mantissa: 0x a 1da4 7cb5 34db\n"
      ". Exponent: 0x266\n. Exponent - 0x3ff = -409\n[/IeeeDouble]\n",
      _s);
  string_delete(&_s);
}

TEST(Double, FromIeeeDouble) {
  IeeeDouble i;

  i.negative = 0;
  i.exponent = 0;
  i.mantissa = 0;
  EXPECT_DEQ(0, double_from_ieee_double(i));

  i.negative = 1;
  i.exponent = 0x3fe;
  i.mantissa = 0x8000000000000;
  EXPECT_DEQ(-0.75, double_from_ieee_double(i));

  i.negative = 1;
  i.exponent = 0x3ff;
  i.mantissa = 0x0;
  EXPECT_DEQ(-1, double_from_ieee_double(i));

  i.negative = 0;
  i.exponent = 0x400;
  i.mantissa = 0x0;
  EXPECT_DEQ(2, double_from_ieee_double(i));

  i.negative = 1;
  i.exponent = 0x400;
  i.mantissa = 0x0;
  EXPECT_DEQ(-2, double_from_ieee_double(i));

  i.negative = 0;
  i.exponent = 0x3fe;
  i.mantissa = 0x0;
  EXPECT_DEQ(0.5, double_from_ieee_double(i));

  i.negative = 1;
  i.exponent = 0x3fe;
  i.mantissa = 0x0;
  EXPECT_DEQ(-0.5, double_from_ieee_double(i));

  i.negative = 0;
  i.exponent = 0x400;
  i.mantissa = 0x8000000000000;
  EXPECT_DEQ(3, double_from_ieee_double(i));

  i.negative = 1;
  i.exponent = 0x400;
  i.mantissa = 0x8000000000000;
  EXPECT_DEQ(-3, double_from_ieee_double(i));

  i.negative = 1;
  i.exponent = 0x3fe;
  i.mantissa = 0x8000000000000;
  EXPECT_DEQ(-0.75, double_from_ieee_double(i));

  i.negative = 0;
  i.exponent = 0x3fd;
  i.mantissa = 0x5555555555555;
  EXPECT_DEQ(1.0 / 3, double_from_ieee_double(i));

  i.negative = 1;
  i.exponent = 0x266;
  i.mantissa = 0xa1da47cb534db;
  EXPECT_DEQ(-1.234567890123456e-123, double_from_ieee_double(i));

  i.negative = 1;
  i.exponent = 0x7fe;
  i.mantissa = 0xfffffffffffff;
  EXPECT_DEQ(-DBL_MAX, double_from_ieee_double(i));

  i.negative = 1;
  i.exponent = 0x1;
  i.mantissa = 0x0;
  EXPECT_DEQ(-DBL_MIN, double_from_ieee_double(i));

  i.negative = 0;
  i.exponent = 0x1;
  i.mantissa = 0x0;
  EXPECT_DEQ(DBL_MIN, double_from_ieee_double(i));

  i.negative = 0;
  i.exponent = 0x7fe;
  i.mantissa = 0xfffffffffffff;
  EXPECT_DEQ(DBL_MAX, double_from_ieee_double(i));
}

TEST(Ulong, FromIeeeDouble) {
  IeeeDouble i;

  i.negative = 0;
  i.exponent = 0;
  i.mantissa = 0;
  EXPECT_ULEQ(0x0, ulong_from_ieee_double(i));

  i.negative = 1;
  i.exponent = 0x7ff;
  i.mantissa = 0xfffffffffffff;
  EXPECT_ULEQ(0xffffffffffffffff, ulong_from_ieee_double(i));

  i.negative = 1;
  i.exponent = 0x3fe;
  i.mantissa = 0x8000000000000;
  EXPECT_ULEQ(0xbfe8000000000000, ulong_from_ieee_double(i));

  i.negative = 1;
  i.exponent = 0x29e;
  i.mantissa = 0xedb9c5317a045;
  EXPECT_ULEQ(0xa9eedb9c5317a045, ulong_from_ieee_double(i));
}

TEST(Ulong, FromDouble) {
  EXPECT_ULEQ(0x0, ulong_from_double(0));
  EXPECT_ULEQ(0x8000000000000000, ulong_from_double(double_negative_zero()));

  EXPECT_ULEQ(0x3ff0000000000000, ulong_from_double(1));
  EXPECT_ULEQ(0xbff0000000000000, ulong_from_double(-1));

  EXPECT_ULEQ(0x4000000000000000, ulong_from_double(2));
  EXPECT_ULEQ(0xc000000000000000, ulong_from_double(-2));

  EXPECT_ULEQ(0x4008000000000000, ulong_from_double(3));
  EXPECT_ULEQ(0xc008000000000000, ulong_from_double(-3));

  EXPECT_ULEQ(0x3fe0000000000000, ulong_from_double(0.5));
  EXPECT_ULEQ(0xbfe0000000000000, ulong_from_double(-0.5));

  EXPECT_ULEQ(0x433fffffffffffff, ulong_from_double(0x1fffffffffffff));
  EXPECT_ULEQ(0xc33fffffffffffff, ulong_from_double(-0x1fffffffffffff));

  EXPECT_ULEQ(0x4340000000000000, ulong_from_double(0x20000000000000));
  EXPECT_ULEQ(0xc340000000000000, ulong_from_double(-0x20000000000000));

  EXPECT_ULEQ(0x4340000000000000, ulong_from_double((double)0x20000000000001));
  EXPECT_ULEQ(0xc340000000000000, ulong_from_double(-(double)0x20000000000001));

  EXPECT_ULEQ(0x4340000000000001, ulong_from_double((double)0x20000000000002));
  EXPECT_ULEQ(0xc340000000000001, ulong_from_double(-(double)0x20000000000002));

  EXPECT_ULEQ(0x266a1da47cb534db, ulong_from_double(1.234567890123456e-123));
  EXPECT_ULEQ(0xa66a1da47cb534db, ulong_from_double(-1.234567890123456e-123));

  EXPECT_ULEQ(0xffefffffffffffff, ulong_from_double(-DBL_MAX));
  EXPECT_ULEQ(0x8010000000000000, ulong_from_double(-DBL_MIN));
  EXPECT_ULEQ(0x0010000000000000, ulong_from_double(DBL_MIN));
  EXPECT_ULEQ(0x7fefffffffffffff, ulong_from_double(DBL_MAX));

  EXPECT_ULEQ(0x1, ulong_from_double(double_subnormal()));
  EXPECT_ULEQ(
      0x7ff0000000000000,
      ulong_from_double(double_positive_infinity()));
  EXPECT_ULEQ(
      0xfff0000000000000,
      ulong_from_double(double_negative_infinity()));
  EXPECT_ULEQ(0X7ff0000000000001, ulong_from_double(double_nan()));
}

TEST(IeeeDouble, FromUlong) {
  IeeeDouble i;

  i = ieee_double_from_ulong(0x0);
  EXPECT_ULEQ(0, i.negative);
  EXPECT_ULEQ(0, i.exponent);
  EXPECT_ULEQ(0, i.mantissa);

  i = ieee_double_from_ulong(0xffffffffffffffff);
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x7ff, i.exponent);
  EXPECT_ULEQ(0xfffffffffffff, i.mantissa);

  i = ieee_double_from_ulong(0xbfe8000000000000);
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x3fe, i.exponent);
  EXPECT_ULEQ(0x8000000000000, i.mantissa);

  i = ieee_double_from_ulong(0xa9eedb9c5317a045);
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x29e, i.exponent);
  EXPECT_ULEQ(0xedb9c5317a045, i.mantissa);
}

TEST(Ulong, FromDouble) {
  EXPECT_DEQ(0, double_from_ulong(0x0));
  EXPECT_TRUE(double_is_negative_zero(double_from_ulong(0x8000000000000000)));

  EXPECT_DEQ(1, double_from_ulong(0x3ff0000000000000));
  EXPECT_DEQ(-1, double_from_ulong(0xbff0000000000000));

  EXPECT_DEQ(2, double_from_ulong(0x4000000000000000));
  EXPECT_DEQ(-2, double_from_ulong(0xc000000000000000));

  EXPECT_DEQ(3, double_from_ulong(0x4008000000000000));
  EXPECT_DEQ(-3, double_from_ulong(0xc008000000000000));

  EXPECT_DEQ(0.5, double_from_ulong(0x3fe0000000000000));
  EXPECT_DEQ(-0.5, double_from_ulong(0xbfe0000000000000));

  EXPECT_DEQ(0x1fffffffffffff, double_from_ulong(0x433fffffffffffff));
  EXPECT_DEQ(-0x1fffffffffffff, double_from_ulong(0xc33fffffffffffff));

  EXPECT_DEQ(0x20000000000000, double_from_ulong(0x4340000000000000));
  EXPECT_DEQ(-0x20000000000000, double_from_ulong(0xc340000000000000));

  EXPECT_DEQ((double)0x20000000000002, double_from_ulong(0x4340000000000001));
  EXPECT_DEQ(-(double)0x20000000000002, double_from_ulong(0xc340000000000001));

  EXPECT_DEQ(1.234567890123456e-123, double_from_ulong(0x266a1da47cb534db));
  EXPECT_DEQ(-1.234567890123456e-123, double_from_ulong(0xa66a1da47cb534db));

  EXPECT_DEQ(-DBL_MAX, double_from_ulong(0xffefffffffffffff));
  EXPECT_DEQ(-DBL_MIN, double_from_ulong(0x8010000000000000));
  EXPECT_DEQ(DBL_MIN, double_from_ulong(0x0010000000000000));
  EXPECT_DEQ(DBL_MAX, double_from_ulong(0x7fefffffffffffff));

  EXPECT_TRUE(double_is_subnormal(double_from_ulong(0x1)));
  EXPECT_TRUE(
      double_is_positive_infinity(double_from_ulong(0x7ff0000000000000)));
  EXPECT_TRUE(
      double_is_negative_infinity(double_from_ulong(0xfff0000000000000)));
}

TEST(IeeeDouble, SignificantBits) {
  IeeeDouble i;

  EXPECT_ULEQ(0, ieee_double_significant_bits(ieee_double_make(0)));
  EXPECT_ULEQ(0, ieee_double_significant_bits(ieee_double_negative_zero()));

  EXPECT_ULEQ(1, ieee_double_significant_bits(ieee_double_make(1)));
  EXPECT_ULEQ(1, ieee_double_significant_bits(ieee_double_make(2)));
  EXPECT_ULEQ(1, ieee_double_significant_bits(ieee_double_make(4)));
  EXPECT_ULEQ(1, ieee_double_significant_bits(ieee_double_make(8)));
  EXPECT_ULEQ(1, ieee_double_significant_bits(ieee_double_make(0.5)));
  EXPECT_ULEQ(1, ieee_double_significant_bits(ieee_double_make(0.25)));
  EXPECT_ULEQ(1, ieee_double_significant_bits(ieee_double_make(0.125)));

  i.negative = 0;
  i.exponent = 0x7fe;
  i.mantissa = 0;
  EXPECT_ULEQ(1, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(1, ieee_double_significant_bits(i));
  i.exponent = 2;
  EXPECT_ULEQ(1, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(1, ieee_double_significant_bits(i));

  EXPECT_ULEQ(1, ieee_double_significant_bits(ieee_double_make(-1)));
  EXPECT_ULEQ(1, ieee_double_significant_bits(ieee_double_make(-2)));
  EXPECT_ULEQ(1, ieee_double_significant_bits(ieee_double_make(-4)));
  EXPECT_ULEQ(1, ieee_double_significant_bits(ieee_double_make(-8)));
  EXPECT_ULEQ(1, ieee_double_significant_bits(ieee_double_make(-0.5)));
  EXPECT_ULEQ(1, ieee_double_significant_bits(ieee_double_make(-0.25)));
  EXPECT_ULEQ(1, ieee_double_significant_bits(ieee_double_make(-0.125)));

  i.negative = 1;
  i.exponent = 0x7fe;
  EXPECT_ULEQ(1, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(1, ieee_double_significant_bits(i));
  i.exponent = 2;
  EXPECT_ULEQ(1, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(1, ieee_double_significant_bits(i));

  EXPECT_ULEQ(2, ieee_double_significant_bits(ieee_double_make(3)));
  EXPECT_ULEQ(2, ieee_double_significant_bits(ieee_double_make(6)));
  EXPECT_ULEQ(2, ieee_double_significant_bits(ieee_double_make(0xc)));
  EXPECT_ULEQ(2, ieee_double_significant_bits(ieee_double_make(0x18)));
  EXPECT_ULEQ(2, ieee_double_significant_bits(ieee_double_make(0x30)));

  EXPECT_ULEQ(2, ieee_double_significant_bits(ieee_double_make(-3)));
  EXPECT_ULEQ(2, ieee_double_significant_bits(ieee_double_make(-6)));
  EXPECT_ULEQ(2, ieee_double_significant_bits(ieee_double_make(-0xc)));
  EXPECT_ULEQ(2, ieee_double_significant_bits(ieee_double_make(-0x18)));
  EXPECT_ULEQ(2, ieee_double_significant_bits(ieee_double_make(-0x30)));

  i.negative = 0;
  i.exponent = 0x7fe;
  i.mantissa = 0x8000000000000;
  EXPECT_ULEQ(2, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(2, ieee_double_significant_bits(i));
  i.exponent = 2;
  EXPECT_ULEQ(2, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(2, ieee_double_significant_bits(i));

  i.negative = 1;
  i.exponent = 0x7fe;
  EXPECT_ULEQ(2, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(2, ieee_double_significant_bits(i));
  i.exponent = 2;
  EXPECT_ULEQ(2, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(2, ieee_double_significant_bits(i));

  i.negative = 0;
  i.exponent = 0x7fe;
  i.mantissa = 0x1;
  EXPECT_ULEQ(53, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(53, ieee_double_significant_bits(i));
  i.exponent = 2;
  EXPECT_ULEQ(53, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(53, ieee_double_significant_bits(i));

  i.negative = 1;
  i.exponent = 0x7fe;
  EXPECT_ULEQ(53, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(53, ieee_double_significant_bits(i));
  i.exponent = 2;
  EXPECT_ULEQ(53, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(53, ieee_double_significant_bits(i));

  i.negative = 0;
  i.exponent = 0x7fe;
  i.mantissa = 0x2;
  EXPECT_ULEQ(52, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(52, ieee_double_significant_bits(i));
  i.exponent = 2;
  EXPECT_ULEQ(52, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(52, ieee_double_significant_bits(i));

  i.negative = 1;
  i.exponent = 0x7fe;
  EXPECT_ULEQ(52, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(52, ieee_double_significant_bits(i));
  i.exponent = 2;
  EXPECT_ULEQ(52, ieee_double_significant_bits(i));
  i.exponent--;
  EXPECT_ULEQ(52, ieee_double_significant_bits(i));
}

TEST(Double, SignificantBits) {
  EXPECT_ULEQ(0, double_significant_bits(0));
  EXPECT_ULEQ(0, double_significant_bits(double_negative_zero()));

  EXPECT_ULEQ(1, double_significant_bits(1));
  EXPECT_ULEQ(1, double_significant_bits(2));
  EXPECT_ULEQ(1, double_significant_bits(4));
  EXPECT_ULEQ(1, double_significant_bits(8));
  EXPECT_ULEQ(1, double_significant_bits(0.5));
  EXPECT_ULEQ(1, double_significant_bits(0.25));
  EXPECT_ULEQ(1, double_significant_bits(0.125));

  EXPECT_ULEQ(1, double_significant_bits(-1));
  EXPECT_ULEQ(1, double_significant_bits(-2));
  EXPECT_ULEQ(1, double_significant_bits(-4));
  EXPECT_ULEQ(1, double_significant_bits(-8));
  EXPECT_ULEQ(1, double_significant_bits(-0.5));
  EXPECT_ULEQ(1, double_significant_bits(-0.25));
  EXPECT_ULEQ(1, double_significant_bits(-0.125));

  EXPECT_ULEQ(53, double_significant_bits(-DBL_MAX));
  EXPECT_ULEQ(1, double_significant_bits(-DBL_MIN));
  EXPECT_ULEQ(1, double_significant_bits(DBL_MIN));
  EXPECT_ULEQ(53, double_significant_bits(DBL_MAX));

  EXPECT_ULEQ(2, double_significant_bits(3));
  EXPECT_ULEQ(2, double_significant_bits(6));
  EXPECT_ULEQ(2, double_significant_bits(0xc));
  EXPECT_ULEQ(2, double_significant_bits(0x18));
  EXPECT_ULEQ(2, double_significant_bits(0x30));

  EXPECT_ULEQ(2, double_significant_bits(-3));
  EXPECT_ULEQ(2, double_significant_bits(-6));
  EXPECT_ULEQ(2, double_significant_bits(-0xc));
  EXPECT_ULEQ(2, double_significant_bits(-0x18));
  EXPECT_ULEQ(2, double_significant_bits(-0x30));

  EXPECT_ULEQ(53, double_significant_bits(0x1fffffffffffff));
  EXPECT_ULEQ(53, double_significant_bits(-0x1fffffffffffff));

  EXPECT_ULEQ(52, double_significant_bits(0xfffffffffffff));
  EXPECT_ULEQ(52, double_significant_bits(-0xfffffffffffff));

  EXPECT_ULEQ(53, double_significant_bits(0x3ffffffffffffe));
  EXPECT_ULEQ(53, double_significant_bits(-0x3ffffffffffffe));

  EXPECT_ULEQ(4, double_significant_bits(0xf000));
  EXPECT_ULEQ(4, double_significant_bits(-0xf000));
}

TEST(IeeeDouble, NegativeZero) {
  IeeeDouble i;

  i = ieee_double_negative_zero();
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x0, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);
}

TEST(Double, NegativeZero) {
  IeeeDouble i;

  i = ieee_double_make(double_negative_zero());
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x0, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);
}

TEST(IeeeDouble, PositiveInfinity) {
  IeeeDouble i;

  i = ieee_double_positive_infinity();
  EXPECT_ULEQ(0, i.negative);
  EXPECT_ULEQ(0x7ff, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);
}

TEST(Double, PositiveInfinity) {
  IeeeDouble i;

  i = ieee_double_make(double_positive_infinity());
  EXPECT_ULEQ(0, i.negative);
  EXPECT_ULEQ(0x7ff, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);
}

TEST(IeeeDouble, NegativeInfinity) {
  IeeeDouble i;

  i = ieee_double_negative_infinity();
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x7ff, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);
}

TEST(Double, NegativeInfinity) {
  IeeeDouble i;

  i = ieee_double_make(double_negative_infinity());
  EXPECT_ULEQ(1, i.negative);
  EXPECT_ULEQ(0x7ff, i.exponent);
  EXPECT_ULEQ(0x0, i.mantissa);
}

TEST(IeeeDouble, Nan) {
  IeeeDouble i;

  i = ieee_double_nan();
  EXPECT_ULEQ(0x7ff, i.exponent);
  EXPECT_TRUE(i.mantissa != 0x0);
}

TEST(Double, Nan) {
  IeeeDouble i;

  i = ieee_double_make(double_nan());
  EXPECT_ULEQ(0x7ff, i.exponent);
  EXPECT_TRUE(i.mantissa != 0x0);
}

TEST(IeeeDouble, Subnormal) {
  IeeeDouble i;

  i = ieee_double_subnormal();
  EXPECT_ULEQ(0x0, i.exponent);
  EXPECT_TRUE(i.mantissa != 0x0);
}

TEST(Double, Subnormal) {
  IeeeDouble i;

  i = ieee_double_make(double_subnormal());
  EXPECT_ULEQ(0x0, i.exponent);
  EXPECT_TRUE(i.mantissa != 0x0);
}

TEST(IeeeDouble, IsNegativeZero) {
  EXPECT_TRUE(ieee_double_is_negative_zero(ieee_double_negative_zero()));
  EXPECT_FALSE(ieee_double_is_negative_zero(ieee_double_make(0)));
  EXPECT_FALSE(ieee_double_is_negative_zero(ieee_double_make(-0.123)));
  EXPECT_FALSE(ieee_double_is_negative_zero(ieee_double_subnormal()));
}

TEST(Double, IsNegativeZero) {
  EXPECT_TRUE(double_is_negative_zero(double_negative_zero()));
  EXPECT_FALSE(double_is_negative_zero(0));
  EXPECT_FALSE(double_is_negative_zero(-0.123));
  EXPECT_FALSE(double_is_negative_zero(double_subnormal()));
}

TEST(IeeeDouble, IsPositiveInfinity) {
  EXPECT_TRUE(
      ieee_double_is_positive_infinity(ieee_double_positive_infinity()));
  EXPECT_FALSE(ieee_double_is_positive_infinity(ieee_double_make(0)));
  EXPECT_FALSE(ieee_double_is_positive_infinity(ieee_double_make(-0.123)));
  EXPECT_FALSE(
      ieee_double_is_positive_infinity(ieee_double_negative_infinity()));
  EXPECT_FALSE(ieee_double_is_positive_infinity(ieee_double_nan()));
}

TEST(Double, IsPositiveInfinity) {
  EXPECT_TRUE(double_is_positive_infinity(double_positive_infinity()));
  EXPECT_FALSE(double_is_positive_infinity(0));
  EXPECT_FALSE(double_is_positive_infinity(-0.123));
  EXPECT_FALSE(double_is_positive_infinity(double_negative_infinity()));
  EXPECT_FALSE(double_is_positive_infinity(double_nan()));
}

TEST(IeeeDouble, IsNegativeInfinity) {
  EXPECT_TRUE(
      ieee_double_is_negative_infinity(ieee_double_negative_infinity()));
  EXPECT_FALSE(ieee_double_is_negative_infinity(ieee_double_make(0)));
  EXPECT_FALSE(ieee_double_is_negative_infinity(ieee_double_make(-0.123)));
  EXPECT_FALSE(
      ieee_double_is_negative_infinity(ieee_double_positive_infinity()));
  EXPECT_FALSE(ieee_double_is_negative_infinity(ieee_double_nan()));
}

TEST(Double, IsNegativeInfinity) {
  EXPECT_TRUE(double_is_negative_infinity(double_negative_infinity()));
  EXPECT_FALSE(double_is_negative_infinity(0));
  EXPECT_FALSE(double_is_negative_infinity(-0.123));
  EXPECT_FALSE(double_is_negative_infinity(double_positive_infinity()));
  EXPECT_FALSE(double_is_negative_infinity(double_nan()));
}

TEST(IeeeDouble, IsNan) {
  EXPECT_TRUE(ieee_double_is_nan(ieee_double_nan()));
  EXPECT_FALSE(ieee_double_is_nan(ieee_double_make(0)));
  EXPECT_FALSE(ieee_double_is_nan(ieee_double_positive_infinity()));
  EXPECT_FALSE(ieee_double_is_nan(ieee_double_negative_infinity()));
}

TEST(Double, IsNan) {
  EXPECT_TRUE(double_is_nan(double_nan()));
  EXPECT_FALSE(double_is_nan(0));
  EXPECT_FALSE(double_is_nan(double_positive_infinity()));
  EXPECT_FALSE(double_is_nan(double_negative_infinity()));
}

TEST(IeeeDouble, IsSubnormal) {
  EXPECT_TRUE(ieee_double_is_subnormal(ieee_double_subnormal()));
  EXPECT_FALSE(ieee_double_is_subnormal(ieee_double_make(0)));
  EXPECT_FALSE(ieee_double_is_subnormal(ieee_double_make(-0.123)));
  EXPECT_FALSE(ieee_double_is_subnormal(ieee_double_make(DBL_MIN)));
  EXPECT_FALSE(ieee_double_is_subnormal(ieee_double_make(-DBL_MIN)));
  EXPECT_FALSE(ieee_double_is_subnormal(ieee_double_positive_infinity()));
  EXPECT_FALSE(ieee_double_is_subnormal(ieee_double_negative_infinity()));
}

TEST(Double, IsSubnormal) {
  EXPECT_TRUE(double_is_subnormal(double_subnormal()));
  EXPECT_TRUE(double_is_subnormal(-double_subnormal()));
  EXPECT_FALSE(double_is_subnormal(0));
  EXPECT_FALSE(double_is_subnormal(-0.123));
  EXPECT_FALSE(double_is_subnormal(DBL_MIN));
  EXPECT_FALSE(double_is_subnormal(-DBL_MIN));
  EXPECT_FALSE(double_is_subnormal(double_positive_infinity()));
  EXPECT_FALSE(double_is_subnormal(double_negative_infinity()));
}

TEST(IeeeDouble, IsNegativeZeroOrSubnormal) {
  EXPECT_TRUE(
      ieee_double_is_negative_zero_or_subnormal(ieee_double_negative_zero()));
  EXPECT_TRUE(
      ieee_double_is_negative_zero_or_subnormal(ieee_double_subnormal()));
  EXPECT_FALSE(ieee_double_is_negative_zero_or_subnormal(ieee_double_make(0)));
  EXPECT_FALSE(
      ieee_double_is_negative_zero_or_subnormal(ieee_double_make(DBL_MIN)));
  EXPECT_FALSE(
      ieee_double_is_negative_zero_or_subnormal(ieee_double_make(-DBL_MIN)));
  EXPECT_FALSE(
      ieee_double_is_negative_zero_or_subnormal(
        ieee_double_positive_infinity()));
  EXPECT_FALSE(
      ieee_double_is_negative_zero_or_subnormal(
        ieee_double_negative_infinity()));
  EXPECT_FALSE(ieee_double_is_negative_zero_or_subnormal(ieee_double_nan()));
}

TEST(Double, IsNegativeZeroOrSubnormal) {
  EXPECT_TRUE(double_is_negative_zero_or_subnormal(double_negative_zero()));
  EXPECT_TRUE(double_is_negative_zero_or_subnormal(double_subnormal()));
  EXPECT_TRUE(double_is_negative_zero_or_subnormal(-double_subnormal()));
  EXPECT_FALSE(double_is_negative_zero_or_subnormal(0));
  EXPECT_FALSE(double_is_negative_zero_or_subnormal(DBL_MIN));
  EXPECT_FALSE(double_is_negative_zero_or_subnormal(-DBL_MIN));
  EXPECT_FALSE(
      double_is_negative_zero_or_subnormal(double_positive_infinity()));
  EXPECT_FALSE(
      double_is_negative_zero_or_subnormal(double_negative_infinity()));
  EXPECT_FALSE(double_is_negative_zero_or_subnormal(double_nan()));
}

TEST(IeeeDouble, IsInfiniteOrNan) {
  EXPECT_TRUE(ieee_double_is_infinite_or_nan(ieee_double_positive_infinity()));
  EXPECT_TRUE(ieee_double_is_infinite_or_nan(ieee_double_negative_infinity()));
  EXPECT_TRUE(ieee_double_is_infinite_or_nan(ieee_double_nan()));
  EXPECT_FALSE(ieee_double_is_infinite_or_nan(ieee_double_make(0)));
  EXPECT_FALSE(ieee_double_is_infinite_or_nan(ieee_double_make(-0.123)));
  EXPECT_FALSE(ieee_double_is_infinite_or_nan(ieee_double_negative_zero()));
  EXPECT_FALSE(ieee_double_is_infinite_or_nan(ieee_double_subnormal()));
}

TEST(Double, IsInfiniteOrNan) {
  EXPECT_TRUE(double_is_infinite_or_nan(double_positive_infinity()));
  EXPECT_TRUE(double_is_infinite_or_nan(double_negative_infinity()));
  EXPECT_TRUE(double_is_infinite_or_nan(double_nan()));
  EXPECT_FALSE(double_is_infinite_or_nan(0));
  EXPECT_FALSE(double_is_infinite_or_nan(-0.123));
  EXPECT_FALSE(double_is_infinite_or_nan(double_negative_zero()));
  EXPECT_FALSE(double_is_infinite_or_nan(double_subnormal()));
}

FINISH
