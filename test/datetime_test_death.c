#include "datetime_test_death.h"

START_DEATH

TEST(Datetime, Death) {
  char *_s = null;

  EXPECT_DEATH(
      string_new_datetime_current(
        &_s,
        true,
        "",
        "",
        "",
        "",
        true,
        false,
        true,
        false,
        0),
      "Can't specify minutes, seconds or decimal places without time");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_datetime_current(
        &_s,
        false,
        "",
        "",
        "",
        "",
        true,
        false,
        false,
        true,
        0),
      "Can't specify minutes, seconds or decimal places without time");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_datetime_current(
        &_s,
        true,
        "",
        "",
        "",
        "",
        true,
        false,
        false,
        false,
        1),
      "Can't specify minutes, seconds or decimal places without time");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_datetime_current(
        &_s,
        false,
        "",
        "",
        "",
        "",
        true,
        true,
        false,
        true,
        0),
      "Can't specify seconds or decimal places without minutes");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_datetime_current(
        &_s,
        true,
        "",
        "",
        "",
        "",
        true,
        true,
        false,
        false,
        2),
      "Can't specify seconds or decimal places without minutes");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_datetime_current(
        &_s,
        false,
        "",
        "",
        "",
        "",
        true,
        true,
        true,
        false,
        5),
      "Can't specify decimal places without seconds");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_datetime_current(
        &_s,
        false,
        "",
        "",
        "",
        "",
        true,
        true,
        true,
        false,
        7),
      "Decimal places (7) cannot be more than 6");
  string_delete(&_s);
}

FINISH_DEATH
