#include "numbers_test_death.h"

START_DEATH

TEST(Parse, Bool) {
  EXPECT_DEATH(parse_bool(""), "Couldn't parse '' as a bool");
  EXPECT_DEATH(parse_bool("TRUE"), "Couldn't parse 'TRUE' as a bool");
  EXPECT_DEATH(parse_bool("T"), "Couldn't parse 'T' as a bool");
  EXPECT_DEATH(parse_bool("FALSE"), "Couldn't parse 'FALSE' as a bool");
  EXPECT_DEATH(parse_bool("F"), "Couldn't parse 'F' as a bool");
  EXPECT_DEATH(parse_bool("-1"), "Couldn't parse '-1' as a bool");
  EXPECT_DEATH(parse_bool("2"), "Couldn't parse '2' as a bool");
  EXPECT_DEATH(parse_bool("+1"), "Couldn't parse '+1' as a bool");
}

TEST(Parse, Byte) {
  EXPECT_DEATH(parse_byte("-1"), "Couldn't parse '-1' as a byte");
  EXPECT_DEATH(parse_byte("+ 1"), "Couldn't parse '+ 1' as a byte");
  EXPECT_DEATH(parse_byte("1+"), "Couldn't parse '1+' as a byte");
  EXPECT_DEATH(parse_byte("256"), "Couldn't parse '256' as a byte");
  EXPECT_DEATH(
      parse_byte("12334875643875643876587346565536"),
      "Couldn't parse '12,334,875,643,875,643,876,587,346,565,536' as a byte");
  EXPECT_DEATH(parse_byte("12 34"), "Couldn't parse '12 34' as a byte");
  EXPECT_DEATH(parse_byte("12.7"), "Couldn't parse '12.7' as a byte");
  EXPECT_DEATH(parse_byte("4."), "Couldn't parse '4.' as a byte");
  EXPECT_DEATH(parse_byte("5e3"), "Couldn't parse '5e3' as a byte");
}

TEST(Parse, Sbyte) {
  EXPECT_DEATH(parse_sbyte("-+1"), "Couldn't parse '-+1' as an sbyte");
  EXPECT_DEATH(parse_sbyte("- 1"), "Couldn't parse '- 1' as an sbyte");
  EXPECT_DEATH(parse_sbyte("1-"), "Couldn't parse '1-' as an sbyte");
  EXPECT_DEATH(parse_sbyte("128"), "Couldn't parse '128' as an sbyte");
  EXPECT_DEATH(parse_sbyte("-129"), "Couldn't parse '-129' as an sbyte");
  EXPECT_DEATH(
      parse_sbyte("12334875643875643876587346565536"),
      "Couldn't parse '12,334,875,643,875,643,876,587,346,565,536' "
      "as an sbyte");
  EXPECT_DEATH(parse_sbyte("12 3"), "Couldn't parse '12 3' as an sbyte");
  EXPECT_DEATH(parse_sbyte("12.7"), "Couldn't parse '12.7' as an sbyte");
  EXPECT_DEATH(parse_sbyte("4."), "Couldn't parse '4.' as an sbyte");
  EXPECT_DEATH(parse_sbyte("5e3"), "Couldn't parse '5e3' as an sbyte");
}

TEST(Parse, Ushort) {
  EXPECT_DEATH(parse_ushort("-1"), "Couldn't parse '-1' as a ushort");
  EXPECT_DEATH(parse_ushort("+ 1"), "Couldn't parse '+ 1' as a ushort");
  EXPECT_DEATH(parse_ushort("1+"), "Couldn't parse '1+' as a ushort");
  EXPECT_DEATH(parse_ushort("65536"), "Couldn't parse '65,536' as a ushort");
  EXPECT_DEATH(
      parse_ushort("12334875643875643876587346565536"),
      "Couldn't parse '12,334,875,643,875,643,876,587,346,565,536' "
      "as a ushort");
  EXPECT_DEATH(parse_ushort("12 34"), "Couldn't parse '12 34' as a ushort");
  EXPECT_DEATH(parse_ushort("12.7"), "Couldn't parse '12.7' as a ushort");
  EXPECT_DEATH(parse_ushort("4."), "Couldn't parse '4.' as a ushort");
  EXPECT_DEATH(parse_ushort("5e3"), "Couldn't parse '5e3' as a ushort");
}

TEST(Parse, Short) {
  EXPECT_DEATH(parse_short("-+1"), "Couldn't parse '-+1' as a short");
  EXPECT_DEATH(parse_short("- 1"), "Couldn't parse '- 1' as a short");
  EXPECT_DEATH(parse_short("1-"), "Couldn't parse '1-' as a short");
  EXPECT_DEATH(parse_short("32768"), "Couldn't parse '32,768' as a short");
  EXPECT_DEATH(parse_short("-32769"), "Couldn't parse '-32,769' as a short");
  EXPECT_DEATH(
      parse_short("12334875643875643876587346565536"),
      "Couldn't parse '12,334,875,643,875,643,876,587,346,565,536' as a short");
  EXPECT_DEATH(parse_short("12 34"), "Couldn't parse '12 34' as a short");
  EXPECT_DEATH(parse_short("12.7"), "Couldn't parse '12.7' as a short");
  EXPECT_DEATH(parse_short("4."), "Couldn't parse '4.' as a short");
  EXPECT_DEATH(parse_short("5e3"), "Couldn't parse '5e3' as a short");
}

TEST(Parse, Uint) {
  EXPECT_DEATH(parse_uint("-1"), "Couldn't parse '-1' as a uint");
  EXPECT_DEATH(parse_uint("+ 1"), "Couldn't parse '+ 1' as a uint");
  EXPECT_DEATH(parse_uint("1+"), "Couldn't parse '1+' as a uint");
  EXPECT_DEATH(
      parse_uint("4294967296"),
      "Couldn't parse '4,294,967,296' as a uint");
  EXPECT_DEATH(
      parse_uint("12334875643875643876587346565536"),
      "Couldn't parse '12,334,875,643,875,643,876,587,346,565,536' as a uint");
  EXPECT_DEATH(parse_uint("12 34"), "Couldn't parse '12 34' as a uint");
  EXPECT_DEATH(parse_uint("12.7"), "Couldn't parse '12.7' as a uint");
  EXPECT_DEATH(parse_uint("4."), "Couldn't parse '4.' as a uint");
  EXPECT_DEATH(parse_uint("5e3"), "Couldn't parse '5e3' as a uint");
}

TEST(Parse, Int) {
  EXPECT_DEATH(parse_int("-+1"), "Couldn't parse '-+1' as an int");
  EXPECT_DEATH(parse_int("- 1"), "Couldn't parse '- 1' as an int");
  EXPECT_DEATH(parse_int("1-"), "Couldn't parse '1-' as an int");
  EXPECT_DEATH(
      parse_int("2147483648"),
      "Couldn't parse '2,147,483,648' as an int");
  EXPECT_DEATH(
      parse_int("-2147483649"),
      "Couldn't parse '-2,147,483,649' as an int");
  EXPECT_DEATH(
      parse_int("12334875643875643876587346565536"),
      "Couldn't parse '12,334,875,643,875,643,876,587,346,565,536' as an int");
  EXPECT_DEATH(parse_int("12 34"), "Couldn't parse '12 34' as an int");
  EXPECT_DEATH(parse_int("12.7"), "Couldn't parse '12.7' as an int");
  EXPECT_DEATH(parse_int("4."), "Couldn't parse '4.' as an int");
  EXPECT_DEATH(parse_int("5e3"), "Couldn't parse '5e3' as an int");
}

TEST(Parse, Ulong) {
  EXPECT_DEATH(parse_ulong("-1"), "Couldn't parse '-1' as a ulong");
  EXPECT_DEATH(parse_ulong("+ 1"), "Couldn't parse '+ 1' as a ulong");
  EXPECT_DEATH(parse_ulong("1+"), "Couldn't parse '1+' as a ulong");
  EXPECT_DEATH(
      parse_ulong("18446744073709551616"),
      "Couldn't parse '18,446,744,073,709,551,616' as a ulong");
  EXPECT_DEATH(
      parse_ulong("12334875643875643876587346565536"),
      "Couldn't parse '12,334,875,643,875,643,876,587,346,565,536' as a ulong");
  EXPECT_DEATH(parse_ulong("12 34"), "Couldn't parse '12 34' as a ulong");
  EXPECT_DEATH(parse_ulong("12.7"), "Couldn't parse '12.7' as a ulong");
  EXPECT_DEATH(parse_ulong("4."), "Couldn't parse '4.' as a ulong");
  EXPECT_DEATH(parse_ulong("5e3"), "Couldn't parse '5e3' as a ulong");
}

TEST(Parse, Long) {
  EXPECT_DEATH(parse_long("-+1"), "Couldn't parse '-+1' as a long");
  EXPECT_DEATH(parse_long("- 1"), "Couldn't parse '- 1' as a long");
  EXPECT_DEATH(parse_long("1-"), "Couldn't parse '1-' as a long");
  EXPECT_DEATH(
      parse_long("9223372036854775808"),
      "Couldn't parse '9,223,372,036,854,775,808' as a long");
  EXPECT_DEATH(
      parse_long("-9223372036854775809"),
      "Couldn't parse '-9,223,372,036,854,775,809' as a long");
  EXPECT_DEATH(
      parse_long("12334875643875643876587346565536"),
      "Couldn't parse '12,334,875,643,875,643,876,587,346,565,536' as a long");
  EXPECT_DEATH(parse_long("12 34"), "Couldn't parse '12 34' as a long");
  EXPECT_DEATH(parse_long("12.7"), "Couldn't parse '12.7' as a long");
  EXPECT_DEATH(parse_long("4."), "Couldn't parse '4.' as a long");
  EXPECT_DEATH(parse_long("5e3"), "Couldn't parse '5e3' as a long");
}

TEST(Parse, Double) {
  EXPECT_DEATH(
      parse_double("1.797693134862317e+308"),
      "Couldn't parse '1.797 693 134 862 317 e+308' as a double");
  EXPECT_DEATH(
      parse_double("-1.797693134862317e+308"),
      "Couldn't parse '-1.797 693 134 862 317 e+308' as a double");
  EXPECT_DEATH(parse_double("-+1"), "Couldn't parse '-+1' as a double");
  EXPECT_DEATH(parse_double("+-1"), "Couldn't parse '+-1' as a double");
  EXPECT_DEATH(parse_double("1g3"), "Couldn't parse '1g3' as a double");
  EXPECT_DEATH(parse_double("1e-+2"), "Couldn't parse '1e-+2' as a double");
  EXPECT_DEATH(parse_double("1e+-2"), "Couldn't parse '1e+-2' as a double");
  EXPECT_DEATH(parse_double("1.34a"), "Couldn't parse '1.34a' as a double");
  EXPECT_DEATH(parse_double("1.23.4"), "Couldn't parse '1.23.4' as a double");
}

TEST(Memory, GibibytesToBytes) {
  EXPECT_DEATH(
      gibibytes_to_bytes(-1),
      "Gibibytes (-1) cannot be negative");
  EXPECT_DEATH(
      gibibytes_to_bytes((double)ULONG_MAX + 1),
      "1.844 674 407 370 955 e+19 GiB = 1.980 704 062 856 608 e+28 bytes "
      "is too large to store in a ulong");
}

FINISH_DEATH
