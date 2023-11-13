#include "numbers_test.h"
#include <float.h>
#include <limits.h>

START

TEST(Parse, Byte) {
  EXPECT_ULEQ(0, parse_byte("0"));
  EXPECT_ULEQ(12, parse_byte(" +12"));
  EXPECT_ULEQ(34, parse_byte("34 "));
  EXPECT_ULEQ(034, parse_byte(" 034 "));
  EXPECT_ULEQ(0xff, parse_byte("\t +255\r "));
  EXPECT_ULEQ(0x7e, parse_byte("\t 0x7e "));
}

TEST(Parse, ByteDecimal) {
  EXPECT_ULEQ(0, parse_byte_decimal("0"));
  EXPECT_ULEQ(12, parse_byte_decimal(" +12"));
  EXPECT_ULEQ(34, parse_byte_decimal("34 "));
  EXPECT_ULEQ(34, parse_byte_decimal(" 034 "));
  EXPECT_ULEQ(0xff, parse_byte_decimal("\t +255\r "));
}

TEST(Parse, Sbyte) {
  EXPECT_LEQ(0, parse_sbyte("0"));
  EXPECT_LEQ(-12, parse_sbyte(" -12"));
  EXPECT_LEQ(125, parse_sbyte("+125 "));
  EXPECT_LEQ(-101, parse_sbyte("\t -101\r "));
  EXPECT_LEQ(0x7f, parse_sbyte(" \f127 \n\n "));
  EXPECT_LEQ(-0x80, parse_sbyte(" \f -128 \n\n "));
}

TEST(Parse, Ushort) {
  EXPECT_ULEQ(0, parse_ushort("0"));
  EXPECT_ULEQ(12, parse_ushort(" +12"));
  EXPECT_ULEQ(345, parse_ushort("345 "));
  EXPECT_ULEQ(0345, parse_ushort(" 0345 "));
  EXPECT_ULEQ(6789, parse_ushort("\t +6789\r "));
  EXPECT_ULEQ(0xffff, parse_ushort(" \f65535 \n\n "));
  EXPECT_ULEQ(0x1234, parse_ushort(" 0x001234 "));
}

TEST(Parse, Short) {
  EXPECT_LEQ(0, parse_short("0"));
  EXPECT_LEQ(-12, parse_short(" -12"));
  EXPECT_LEQ(345, parse_short("+345 "));
  EXPECT_LEQ(-6789, parse_short("\t -6789\r "));
  EXPECT_LEQ(0x7fff, parse_short(" \f32767 \n\n "));
  EXPECT_LEQ(-0x8000, parse_short(" \f -32768 \n\n "));
}

TEST(Parse, Uint) {
  EXPECT_ULEQ(0, parse_uint("0"));
  EXPECT_ULEQ(12, parse_uint(" +12"));
  EXPECT_ULEQ(345, parse_uint("345 "));
  EXPECT_ULEQ(0345, parse_uint("  0345 "));
  EXPECT_ULEQ(6789, parse_uint("\t +6789\r "));
  EXPECT_ULEQ(0x1234abcd, parse_uint(" 0X1234ABCD\n "));
  EXPECT_ULEQ(0xffffffff, parse_uint(" \f4294967295 \n\n "));
}

TEST(Parse, Int) {
  EXPECT_LEQ(0, parse_int("0"));
  EXPECT_LEQ(-12, parse_int(" -12"));
  EXPECT_LEQ(345, parse_int("+345 "));
  EXPECT_LEQ(-6789, parse_int("\t -6789\r "));
  EXPECT_LEQ(0x7fffffff, parse_int(" \f2147483647 \n\n "));
  EXPECT_LEQ(-0x80000000L, parse_int(" \f -2147483648 \n\n "));
}

TEST(Parse, Ulong) {
  EXPECT_ULEQ(0, parse_ulong("0"));
  EXPECT_ULEQ(12, parse_ulong(" +12"));
  EXPECT_ULEQ(345, parse_ulong("345 "));
  EXPECT_ULEQ(034567, parse_ulong("034567 "));
  EXPECT_ULEQ(6789, parse_ulong("\t +6789\r "));
  EXPECT_ULEQ(0x123456789abcdef0, parse_ulong(" 0x123456789abcdef0"));
  EXPECT_ULEQ(0xffffffffffffffff, parse_ulong(" \f18446744073709551615 \n\n "));
}

TEST(Parse, Long) {
  EXPECT_LEQ(0, parse_long("0"));
  EXPECT_LEQ(-12, parse_long(" -12"));
  EXPECT_LEQ(345, parse_long("+345 "));
  EXPECT_LEQ(-6789, parse_long("\t -6789\r "));
  EXPECT_LEQ(0x7fffffffffffffff, parse_long(" \f9223372036854775807 \n\n "));
  EXPECT_LEQ(-0x8000000000000000, parse_long(" \f -9223372036854775808 \n\n "));
}

TEST(Parse, Double) {
  EXPECT_DEQ(0, parse_double("0"));
  EXPECT_DEQ(0, parse_double(" 0."));
  EXPECT_DEQ(0, parse_double("0.0 "));
  EXPECT_DEQ(0, parse_double("\t+0 "));
  EXPECT_DEQ(0, parse_double(" \f -0 \n\r\f "));
  EXPECT_DEQ(0, parse_double("0e1"));
  EXPECT_DEQ(0, parse_double("0e+1"));
  EXPECT_DEQ(0, parse_double("0e-1"));
  EXPECT_DEQ(0, parse_double("0.E1"));
  EXPECT_DEQ(0, parse_double(" 0.0E13"));
  EXPECT_DEQ(-12, parse_double("\t-12"));
  EXPECT_DEQ(345, parse_double("+345  "));
  EXPECT_DEQ(-0.01234, parse_double(" -12.34E-3"));
  EXPECT_NEAR(1.234567890123457e24, parse_double("123456789012345678e7"), 3e8);
  EXPECT_DEQ(
      1.7976931348623157e+308,
      parse_double("1.7976931348623157E+308"));
  EXPECT_DEQ(
      -1.7976931348623157e+308,
      parse_double("-1.7976931348623157e+308"));
  EXPECT_TRUE(double_is_negative_zero(parse_double("-0")));
  EXPECT_TRUE(double_is_positive_infinity(parse_double("Inf")));
  EXPECT_TRUE(double_is_negative_infinity(parse_double("-Inf")));
  EXPECT_TRUE(double_is_nan(parse_double("NaN")));
}

TEST(Parse, TryByte) {
  byte b;

  EXPECT_TRUE(try_parse_byte(&b, "0"));
  EXPECT_ULEQ(0, b);
  EXPECT_TRUE(try_parse_byte(&b, " +12"));
  EXPECT_ULEQ(12, b);
  EXPECT_TRUE(try_parse_byte(&b, "034 "));
  EXPECT_ULEQ(034, b);
  EXPECT_TRUE(try_parse_byte(&b, "\t +255\r "));
  EXPECT_ULEQ(0xff, b);
  EXPECT_TRUE(try_parse_byte(&b, "\t 0x7f\r "));
  EXPECT_ULEQ(0x7f, b);
  EXPECT_FALSE(try_parse_byte(&b, ""));
  EXPECT_FALSE(try_parse_byte(&b, "-1"));
  EXPECT_FALSE(try_parse_byte(&b, "+ 1"));
  EXPECT_FALSE(try_parse_byte(&b, "1+"));
  EXPECT_FALSE(try_parse_byte(&b, "256"));
  EXPECT_FALSE(try_parse_byte(&b, "12334875643875643876587346565536"));
  EXPECT_FALSE(try_parse_byte(&b, "12 34"));
  EXPECT_FALSE(try_parse_byte(&b, "12.7"));
  EXPECT_FALSE(try_parse_byte(&b, "4."));
  EXPECT_FALSE(try_parse_byte(&b, "5e2"));
}

TEST(Parse, TryByteDecimal) {
  byte b;

  EXPECT_TRUE(try_parse_byte_decimal(&b, "0"));
  EXPECT_ULEQ(0, b);
  EXPECT_TRUE(try_parse_byte_decimal(&b, " +12"));
  EXPECT_ULEQ(12, b);
  EXPECT_TRUE(try_parse_byte_decimal(&b, "034 "));
  EXPECT_ULEQ(34, b);
  EXPECT_TRUE(try_parse_byte_decimal(&b, "\t +255\r "));
  EXPECT_ULEQ(0xff, b);
  EXPECT_FALSE(try_parse_byte_decimal(&b, "\t 0x7f\r "));
  EXPECT_FALSE(try_parse_byte_decimal(&b, ""));
  EXPECT_FALSE(try_parse_byte_decimal(&b, "-1"));
  EXPECT_FALSE(try_parse_byte_decimal(&b, "+ 1"));
  EXPECT_FALSE(try_parse_byte_decimal(&b, "1+"));
  EXPECT_FALSE(try_parse_byte_decimal(&b, "256"));
  EXPECT_FALSE(try_parse_byte_decimal(&b, "12334875643875643876587346565536"));
  EXPECT_FALSE(try_parse_byte_decimal(&b, "12 34"));
  EXPECT_FALSE(try_parse_byte_decimal(&b, "12.7"));
  EXPECT_FALSE(try_parse_byte_decimal(&b, "4."));
  EXPECT_FALSE(try_parse_byte_decimal(&b, "5e2"));
}

TEST(Parse, TrySbyte) {
  sbyte s;

  EXPECT_TRUE(try_parse_sbyte(&s, "0"));
  EXPECT_LEQ(0, s);
  EXPECT_TRUE(try_parse_sbyte(&s, " -12"));
  EXPECT_LEQ(-12, s);
  EXPECT_TRUE(try_parse_sbyte(&s, "+123 "));
  EXPECT_LEQ(123, s);
  EXPECT_TRUE(try_parse_sbyte(&s, "\t -102\r "));
  EXPECT_LEQ(-102, s);
  EXPECT_TRUE(try_parse_sbyte(&s, " \f127 \n\n "));
  EXPECT_LEQ(0x7f, s);
  EXPECT_TRUE(try_parse_sbyte(&s, " \f -128 \n\n "));
  EXPECT_LEQ(-0x80, s);
  EXPECT_FALSE(try_parse_sbyte(&s, "-+1"));
  EXPECT_FALSE(try_parse_sbyte(&s, "- 1"));
  EXPECT_FALSE(try_parse_sbyte(&s, "1-"));
  EXPECT_FALSE(try_parse_sbyte(&s, "128"));
  EXPECT_FALSE(try_parse_sbyte(&s, "-129"));
  EXPECT_FALSE(try_parse_sbyte(&s, "12334875643875643876587346565536"));
  EXPECT_FALSE(try_parse_sbyte(&s, "12 3"));
  EXPECT_FALSE(try_parse_sbyte(&s, "12.7"));
  EXPECT_FALSE(try_parse_sbyte(&s, "4."));
  EXPECT_FALSE(try_parse_sbyte(&s, "5e1"));
}

TEST(Parse, TryUshort) {
  ushort u;

  EXPECT_TRUE(try_parse_ushort(&u, "0"));
  EXPECT_ULEQ(0, u);
  EXPECT_TRUE(try_parse_ushort(&u, " +12"));
  EXPECT_ULEQ(12, u);
  EXPECT_TRUE(try_parse_ushort(&u, "345 "));
  EXPECT_ULEQ(345, u);
  EXPECT_TRUE(try_parse_ushort(&u, "\t +6789\r "));
  EXPECT_ULEQ(6789, u);
  EXPECT_TRUE(try_parse_ushort(&u, " \f65535 \n\n "));
  EXPECT_ULEQ(0xffff, u);
  EXPECT_FALSE(try_parse_ushort(&u, ""));
  EXPECT_FALSE(try_parse_ushort(&u, "-1"));
  EXPECT_FALSE(try_parse_ushort(&u, "+ 1"));
  EXPECT_FALSE(try_parse_ushort(&u, "1+"));
  EXPECT_FALSE(try_parse_ushort(&u, "65536"));
  EXPECT_FALSE(try_parse_ushort(&u, "12334875643875643876587346565536"));
  EXPECT_FALSE(try_parse_ushort(&u, "12 34"));
  EXPECT_FALSE(try_parse_ushort(&u, "12.7"));
  EXPECT_FALSE(try_parse_ushort(&u, "4."));
  EXPECT_FALSE(try_parse_ushort(&u, "5e3"));
}

TEST(Parse, TryShort) {
  short s;

  EXPECT_TRUE(try_parse_short(&s, "0"));
  EXPECT_LEQ(0, s);
  EXPECT_TRUE(try_parse_short(&s, " -12"));
  EXPECT_LEQ(-12, s);
  EXPECT_TRUE(try_parse_short(&s, "+345 "));
  EXPECT_LEQ(345, s);
  EXPECT_TRUE(try_parse_short(&s, "\t -6789\r "));
  EXPECT_LEQ(-6789, s);
  EXPECT_TRUE(try_parse_short(&s, " \f32767 \n\n "));
  EXPECT_LEQ(0x7fff, s);
  EXPECT_TRUE(try_parse_short(&s, " \f -32768 \n\n "));
  EXPECT_LEQ(-0x8000, s);
  EXPECT_FALSE(try_parse_short(&s, "-+1"));
  EXPECT_FALSE(try_parse_short(&s, "- 1"));
  EXPECT_FALSE(try_parse_short(&s, "1-"));
  EXPECT_FALSE(try_parse_short(&s, "32768"));
  EXPECT_FALSE(try_parse_short(&s, "-32769"));
  EXPECT_FALSE(try_parse_short(&s, "12334875643875643876587346565536"));
  EXPECT_FALSE(try_parse_short(&s, "12 34"));
  EXPECT_FALSE(try_parse_short(&s, "12.7"));
  EXPECT_FALSE(try_parse_short(&s, "4."));
  EXPECT_FALSE(try_parse_short(&s, "5e3"));
}

TEST(Parse, TryUint) {
  uint u;

  EXPECT_TRUE(try_parse_uint(&u, "0"));
  EXPECT_ULEQ(0, u);
  EXPECT_TRUE(try_parse_uint(&u, " +12"));
  EXPECT_ULEQ(12, u);
  EXPECT_TRUE(try_parse_uint(&u, "345 "));
  EXPECT_ULEQ(345, u);
  EXPECT_TRUE(try_parse_uint(&u, "\t +6789\r "));
  EXPECT_ULEQ(6789, u);
  EXPECT_TRUE(try_parse_uint(&u, " \f4294967295 \n\n "));
  EXPECT_ULEQ(0xffffffff, u);
  EXPECT_FALSE(try_parse_uint(&u, ""));
  EXPECT_FALSE(try_parse_uint(&u, "-1"));
  EXPECT_FALSE(try_parse_uint(&u, "+ 1"));
  EXPECT_FALSE(try_parse_uint(&u, "1+"));
  EXPECT_FALSE(try_parse_uint(&u, "4294967296"));
  EXPECT_FALSE(try_parse_uint(&u, "12334875643875643876587346565536"));
  EXPECT_FALSE(try_parse_uint(&u, "12 34"));
  EXPECT_FALSE(try_parse_uint(&u, "12.7"));
  EXPECT_FALSE(try_parse_uint(&u, "4."));
  EXPECT_FALSE(try_parse_uint(&u, "5e3"));
}

TEST(Parse, TryInt) {
  int i;

  EXPECT_TRUE(try_parse_int(&i, "0"));
  EXPECT_LEQ(0, i);
  EXPECT_TRUE(try_parse_int(&i, " -12"));
  EXPECT_LEQ(-12, i);
  EXPECT_TRUE(try_parse_int(&i, "+345 "));
  EXPECT_LEQ(345, i);
  EXPECT_TRUE(try_parse_int(&i, "\t -6789\r "));
  EXPECT_LEQ(-6789, i);
  EXPECT_TRUE(try_parse_int(&i, " \f2147483647 \n\n "));
  EXPECT_LEQ(0x7fffffff, i);
  EXPECT_TRUE(try_parse_int(&i, " \f -2147483648 \n\n "));
  EXPECT_LEQ(-0x80000000L, i);
  EXPECT_FALSE(try_parse_int(&i, ""));
  EXPECT_FALSE(try_parse_int(&i, "-+1"));
  EXPECT_FALSE(try_parse_int(&i, "- 1"));
  EXPECT_FALSE(try_parse_int(&i, "1-"));
  EXPECT_FALSE(try_parse_int(&i, "2147483648"));
  EXPECT_FALSE(try_parse_int(&i, "-2147483649"));
  EXPECT_FALSE(try_parse_int(&i, "12334875643875643876587346565536"));
  EXPECT_FALSE(try_parse_int(&i, "12 34"));
  EXPECT_FALSE(try_parse_int(&i, "12.7"));
  EXPECT_FALSE(try_parse_int(&i, "4."));
  EXPECT_FALSE(try_parse_int(&i, "5e3"));
}

TEST(Parse, TryUlong) {
  ulong u;

  EXPECT_TRUE(try_parse_ulong(&u, "0"));
  EXPECT_ULEQ(0, u);
  EXPECT_TRUE(try_parse_ulong(&u, " +12"));
  EXPECT_ULEQ(12, u);
  EXPECT_TRUE(try_parse_ulong(&u, "345 "));
  EXPECT_ULEQ(345, u);
  EXPECT_TRUE(try_parse_ulong(&u, "\t +6789\r "));
  EXPECT_ULEQ(6789, u);
  EXPECT_TRUE(try_parse_ulong(&u, " \f18446744073709551615 \n\n "));
  EXPECT_ULEQ(0xffffffffffffffff, u);
  EXPECT_FALSE(try_parse_ulong(&u, ""));
  EXPECT_FALSE(try_parse_ulong(&u, "-1"));
  EXPECT_FALSE(try_parse_ulong(&u, "+ 1"));
  EXPECT_FALSE(try_parse_ulong(&u, "1+"));
  EXPECT_FALSE(try_parse_ulong(&u, "18446744073709551616"));
  EXPECT_FALSE(try_parse_ulong(&u, "12334875643875643876587346565536"));
  EXPECT_FALSE(try_parse_ulong(&u, "12 34"));
  EXPECT_FALSE(try_parse_ulong(&u, "12.7"));
  EXPECT_FALSE(try_parse_ulong(&u, "4."));
  EXPECT_FALSE(try_parse_ulong(&u, "5e3"));
}

TEST(Parse, TryLong) {
  long l;

  EXPECT_TRUE(try_parse_long(&l, "0"));
  EXPECT_LEQ(0, l);
  EXPECT_TRUE(try_parse_long(&l, " -12"));
  EXPECT_LEQ(-12, l);
  EXPECT_TRUE(try_parse_long(&l, "+345 "));
  EXPECT_LEQ(345, l);
  EXPECT_TRUE(try_parse_long(&l, "\t -6789\r "));
  EXPECT_LEQ(-6789, l);
  EXPECT_TRUE(try_parse_long(&l, " \f9223372036854775807 \n\n "));
  EXPECT_LEQ(0x7fffffffffffffff, l);
  EXPECT_TRUE(try_parse_long(&l, " \f -9223372036854775808 \n\n "));
  EXPECT_LEQ(-0x8000000000000000, l);
  EXPECT_FALSE(try_parse_long(&l, ""));
  EXPECT_FALSE(try_parse_long(&l, "-+1"));
  EXPECT_FALSE(try_parse_long(&l, "- 1"));
  EXPECT_FALSE(try_parse_long(&l, "1-"));
  EXPECT_FALSE(try_parse_long(&l, "9223372036854775808"));
  EXPECT_FALSE(try_parse_long(&l, "-9223372036854775809"));
  EXPECT_FALSE(try_parse_long(&l, "12334875643875643876587346565536"));
  EXPECT_FALSE(try_parse_long(&l, "12 34"));
  EXPECT_FALSE(try_parse_long(&l, "12.7"));
  EXPECT_FALSE(try_parse_long(&l, "4."));
  EXPECT_FALSE(try_parse_long(&l, "5e3"));
}

TEST(Parse, TryDouble) {
  double d;

  EXPECT_TRUE(try_parse_double(&d, "0"));
  EXPECT_ULEQ(0, d);
  EXPECT_TRUE(try_parse_double(&d, " 0."));
  EXPECT_ULEQ(0, d);
  EXPECT_TRUE(try_parse_double(&d, "0.0 "));
  EXPECT_ULEQ(0, d);
  EXPECT_TRUE(try_parse_double(&d, "\t+0 "));
  EXPECT_ULEQ(0, d);
  EXPECT_TRUE(try_parse_double(&d, " \f -0 \n\r\f "));
  EXPECT_ULEQ(0, d);
  EXPECT_TRUE(try_parse_double(&d, "0e1"));
  EXPECT_ULEQ(0, d);
  EXPECT_TRUE(try_parse_double(&d, "0e+1"));
  EXPECT_ULEQ(0, d);
  EXPECT_TRUE(try_parse_double(&d, "+0e-1"));
  EXPECT_ULEQ(0, d);
  EXPECT_TRUE(try_parse_double(&d, "0.E1"));
  EXPECT_ULEQ(0, d);
  EXPECT_TRUE(try_parse_double(&d, " 0.0e13"));
  EXPECT_DEQ(0, d);
  EXPECT_TRUE(try_parse_double(&d, " -12"));
  EXPECT_DEQ(-12, d);
  EXPECT_TRUE(try_parse_double(&d, "+345 "));
  EXPECT_DEQ(345, d);
  EXPECT_TRUE(try_parse_double(&d, "1.2345"));
  EXPECT_DEQ(1.2345, d);
  EXPECT_TRUE(try_parse_double(&d, " -12.34E-3"));
  EXPECT_DEQ(-0.01234, d);
  EXPECT_TRUE(try_parse_double(&d, "123456789012345678e7"));
  EXPECT_NEAR(1.234567890123457e24, d, 3e8);
  EXPECT_TRUE(try_parse_double(&d, "1.7976931348623157e+308"));
  EXPECT_DEQ(1.7976931348623157e+308, d);
  EXPECT_TRUE(try_parse_double(&d, "-1.7976931348623157e+308"));
  EXPECT_DEQ(-1.7976931348623157e+308, d);
  EXPECT_FALSE(try_parse_double(&d, ""));
  EXPECT_FALSE(try_parse_double(&d, "1.797693134862317e+308"));
  EXPECT_FALSE(try_parse_double(&d, "-1.797693134862317e+308"));
  EXPECT_FALSE(try_parse_double(&d, "-+1"));
  EXPECT_FALSE(try_parse_double(&d, "+-1"));
  EXPECT_FALSE(try_parse_double(&d, "1g3"));
  EXPECT_FALSE(try_parse_double(&d, "1e-+2"));
  EXPECT_FALSE(try_parse_double(&d, "1e+-2"));
  EXPECT_FALSE(try_parse_double(&d, "1.34a"));
  EXPECT_FALSE(try_parse_double(&d, "1.23.4"));
  EXPECT_TRUE(try_parse_double(&d, "-0"));
  EXPECT_TRUE(double_is_negative_zero(d));
  EXPECT_TRUE(try_parse_double(&d, "Inf"));
  EXPECT_TRUE(double_is_positive_infinity(d));
  EXPECT_TRUE(try_parse_double(&d, "Infinity"));
  EXPECT_TRUE(double_is_positive_infinity(d));
  EXPECT_TRUE(try_parse_double(&d, "inf"));
  EXPECT_TRUE(double_is_positive_infinity(d));
  EXPECT_FALSE(try_parse_double(&d, "Infin"));
  EXPECT_TRUE(try_parse_double(&d, "-Inf"));
  EXPECT_TRUE(double_is_negative_infinity(d));
  EXPECT_TRUE(try_parse_double(&d, "-Infinity"));
  EXPECT_TRUE(double_is_negative_infinity(d));
  EXPECT_TRUE(try_parse_double(&d, "-inf"));
  EXPECT_TRUE(double_is_negative_infinity(d));
  EXPECT_FALSE(try_parse_double(&d, "-Infin"));
  EXPECT_TRUE(try_parse_double(&d, "NaN"));
  EXPECT_TRUE(double_is_nan(d));
  EXPECT_TRUE(try_parse_double(&d, "nan"));
  EXPECT_TRUE(double_is_nan(d));
  EXPECT_FALSE(try_parse_double(&d, "Nanna"));
}

TEST(Digits, Ulong) {
  EXPECT_ULEQ(1, ulong_digits(0));
  EXPECT_ULEQ(1, ulong_digits(1));
  EXPECT_ULEQ(1, ulong_digits(9));
  EXPECT_ULEQ(2, ulong_digits(10));
  EXPECT_ULEQ(2, ulong_digits(99));
  EXPECT_ULEQ(3, ulong_digits(100));
  EXPECT_ULEQ(5, ulong_digits(12345));
  EXPECT_ULEQ(5, ulong_digits(12345));
  EXPECT_ULEQ(20, ulong_digits(0xffffffffffffffff));
}

TEST(StringNewSerialized, Ulong) {
  char *_ = null;

  string_new_serialized_ulong(&_, 0);
  EXPECT_STREQ("0x0000000000000000", _);
  string_delete(&_);

  string_new_serialized_ulong(&_, 1);
  EXPECT_STREQ("0x0000000000000001", _);
  string_delete(&_);

  string_new_serialized_ulong(&_, 2);
  EXPECT_STREQ("0x0000000000000002", _);
  string_delete(&_);

  string_new_serialized_ulong(&_, 0xf);
  EXPECT_STREQ("0x000000000000000f", _);
  string_delete(&_);

  string_new_serialized_ulong(&_, ULONG_MAX);
  EXPECT_STREQ("0xffffffffffffffff", _);
  string_delete(&_);

  string_new_serialized_ulong(&_, 0x123456789abcdef0);
  EXPECT_STREQ("0x123456789abcdef0", _);
  string_delete(&_);
}

TEST(StringNewSerialized, Double) {
  char *_ = null;

  string_new_serialized_double(&_, 0);
  EXPECT_STREQ("0x0000000000000000", _);
  string_delete(&_);

  string_new_serialized_double(&_, 1);
  EXPECT_STREQ("0x3ff0000000000000", _);
  string_delete(&_);

  string_new_serialized_double(&_, -1);
  EXPECT_STREQ("0xbff0000000000000", _);
  string_delete(&_);

  string_new_serialized_double(&_, 2);
  EXPECT_STREQ("0x4000000000000000", _);
  string_delete(&_);

  string_new_serialized_double(&_, -2);
  EXPECT_STREQ("0xc000000000000000", _);
  string_delete(&_);

  string_new_serialized_double(&_, -0.75);
  EXPECT_STREQ("0xbfe8000000000000", _);
  string_delete(&_);

  string_new_serialized_double(&_, DBL_MAX);
  EXPECT_STREQ("0x7fefffffffffffff", _);
  string_delete(&_);

  string_new_serialized_double(&_, -DBL_MAX);
  EXPECT_STREQ("0xffefffffffffffff", _);
  string_delete(&_);

  string_new_serialized_double(&_, DBL_MIN);
  EXPECT_STREQ("0x0010000000000000", _);
  string_delete(&_);

  string_new_serialized_double(&_, -DBL_MIN);
  EXPECT_STREQ("0x8010000000000000", _);
  string_delete(&_);

  string_new_serialized_double(&_, double_negative_zero());
  EXPECT_STREQ("0x8000000000000000", _);
  string_delete(&_);

  string_new_serialized_double(&_, double_positive_infinity());
  EXPECT_STREQ("0x7ff0000000000000", _);
  string_delete(&_);

  string_new_serialized_double(&_, double_negative_infinity());
  EXPECT_STREQ("0xfff0000000000000", _);
  string_delete(&_);

  string_new_serialized_double(&_, double_nan());
  EXPECT_STREQ("0x7ff0000000000001", _);
  string_delete(&_);
}

TEST(Memory, GibibytesToBytes) {
  EXPECT_ULEQ(0, gibibytes_to_bytes(0));
  EXPECT_ULEQ(0, gibibytes_to_bytes(0.499 / (1024 * 1024 * 1024)));
  EXPECT_ULEQ(1, gibibytes_to_bytes(1.0 / (1024 * 1024 * 1024)));
  EXPECT_ULEQ(1, gibibytes_to_bytes(0.501 / (1024 * 1024 * 1024)));
  EXPECT_ULEQ(1, gibibytes_to_bytes(1.499 / (1024 * 1024 * 1024)));
  EXPECT_ULEQ(1024 * 1024 * 1024, gibibytes_to_bytes(1));
  EXPECT_ULEQ(512 * 1024 * 1024, gibibytes_to_bytes(0.5));
  EXPECT_ULEQ(3373259426, gibibytes_to_bytes(3.14159265358979));
}

TEST(String, NewMemory) {
  char *_s = null;
  ulong k = 1024;

  string_new_memory(&_s, 0);
  EXPECT_STREQ("0 B", _s);
  string_delete(&_s);

  string_new_memory(&_s, 9);
  EXPECT_STREQ("9 B", _s);
  string_delete(&_s);

  string_new_memory(&_s, 10);
  EXPECT_STREQ("10 B", _s);
  string_delete(&_s);

  string_new_memory(&_s, 99);
  EXPECT_STREQ("99 B", _s);
  string_delete(&_s);

  string_new_memory(&_s, 100);
  EXPECT_STREQ("100 B", _s);
  string_delete(&_s);

  string_new_memory(&_s, 999);
  EXPECT_STREQ("999 B", _s);
  string_delete(&_s);

  string_new_memory(&_s, 1000);
  EXPECT_STREQ("1000 B", _s);
  string_delete(&_s);

  string_new_memory(&_s, 1023);
  EXPECT_STREQ("1023 B", _s);
  string_delete(&_s);

  string_new_memory(&_s, k);
  EXPECT_STREQ("1 KiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 4 * k / 3);
  EXPECT_STREQ("1.33 KiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 8 * k / 3);
  EXPECT_STREQ("2.67 KiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 40 * k / 3);
  EXPECT_STREQ("13.3 KiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 80 * k / 3);
  EXPECT_STREQ("26.7 KiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 400 * k / 3);
  EXPECT_STREQ("133 KiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 800 * k / 3);
  EXPECT_STREQ("267 KiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 999 * k + 511);
  EXPECT_STREQ("999 KiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 999 * k + 512);
  EXPECT_STREQ("1000 KiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 1000 * k);
  EXPECT_STREQ("1000 KiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 1023 * k + 511);
  EXPECT_STREQ("1023 KiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 1023 * k + 512);
  EXPECT_STREQ("1 MiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, k * k);
  EXPECT_STREQ("1 MiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 999 * k * k + 511 * k + 1023);
  EXPECT_STREQ("999 MiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 999 * k * k + 512 * k);
  EXPECT_STREQ("1000 MiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 1000 * k * k);
  EXPECT_STREQ("1000 MiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 1023 * k * k + 511 * k + 1023);
  EXPECT_STREQ("1023 MiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, 1023 * k * k + 512 * k);
  EXPECT_STREQ("1 GiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, k * k * k);
  EXPECT_STREQ("1 GiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, k * k * k * k);
  EXPECT_STREQ("1 TiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, k * k * k * k * k);
  EXPECT_STREQ("1 PiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, k * k * k * k * k * k);
  EXPECT_STREQ("1 EiB", _s);
  string_delete(&_s);

  string_new_memory(&_s, ULONG_MAX);
  EXPECT_STREQ("16 EiB", _s);
  string_delete(&_s);
}

FINISH
