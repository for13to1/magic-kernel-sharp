#include "datetime_test.h"

START

TEST(Datetime, Regular) {
  char *_e = null;
  char *_s = null;
  struct timeval tv = {1393548153, 704907};
  struct tm t;

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "-",
      "boo",
      "ha",
      "whoa",
      true,
      false,
      false,
      false,
      0);
  EXPECT_STREQ("2014-02-28", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "",
      "T",
      "ha",
      "whoa",
      true,
      true,
      false,
      false,
      0);
  EXPECT_STREQ("20140228T00", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "-",
      " ",
      ":",
      "whoa",
      true,
      true,
      true,
      false,
      0);
  EXPECT_STREQ("2014-02-28 00:42", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "/",
      " time ",
      "",
      "whoa",
      true,
      true,
      true,
      true,
      0);
  EXPECT_STREQ("2014/02/28 time 004233", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "-",
      "T",
      ":",
      ".",
      true,
      true,
      true,
      true,
      1);
  EXPECT_STREQ("2014-02-28T00:42:33.7", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "-",
      "T",
      ":",
      ".",
      true,
      true,
      true,
      true,
      2);
  EXPECT_STREQ("2014-02-28T00:42:33.70", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "-",
      "T",
      ":",
      ".",
      true,
      true,
      true,
      true,
      3);
  EXPECT_STREQ("2014-02-28T00:42:33.704", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "-",
      "T",
      ":",
      ".",
      true,
      true,
      true,
      true,
      4);
  EXPECT_STREQ("2014-02-28T00:42:33.7049", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "-",
      "T",
      ":",
      ".",
      true,
      true,
      true,
      true,
      5);
  EXPECT_STREQ("2014-02-28T00:42:33.70490", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "-",
      "T",
      ":",
      ".",
      true,
      true,
      true,
      true,
      6);
  EXPECT_STREQ("2014-02-28T00:42:33.704907", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "-",
      "T",
      ":",
      ".",
      false,
      true,
      true,
      true,
      0);
  EXPECT_STREQ("00:42:33", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "-",
      "T",
      ":",
      ".",
      false,
      true,
      true,
      true,
      1);
  EXPECT_STREQ("00:42:33.7", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "-",
      "T",
      ":",
      ".",
      false,
      true,
      true,
      true,
      2);
  EXPECT_STREQ("00:42:33.70", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "-",
      "T",
      ":",
      ".",
      false,
      true,
      true,
      true,
      3);
  EXPECT_STREQ("00:42:33.704", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "-",
      "T",
      ":",
      ".",
      false,
      true,
      true,
      true,
      4);
  EXPECT_STREQ("00:42:33.7049", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "-",
      "T",
      ":",
      ".",
      false,
      true,
      true,
      true,
      5);
  EXPECT_STREQ("00:42:33.70490", _s);
  string_delete(&_s);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      true,
      "-",
      "T",
      ":",
      ".",
      false,
      true,
      true,
      true,
      6);
  EXPECT_STREQ("00:42:33.704907", _s);
  string_delete(&_s);

  /* Do a single local time one only, since it's basically a copy of the
   * library code.
   */
  t = time_t_to_tm(&tv.tv_sec, false);

  string_new_f(
      &_e,
      "%04d-%02d-%02dT%02d:%02d:%02d.%06d",
      1900 + t.tm_year,
      1 + t.tm_mon,
      t.tm_mday,
      t.tm_hour,
      t.tm_min,
      t.tm_sec,
      tv.tv_usec);

  string_new_datetime_from_timeval(
      &_s,
      &tv,
      false,
      "-",
      "T",
      ":",
      ".",
      true,
      true,
      true,
      true,
      6);
  EXPECT_STREQ(_e, _s);
  string_delete(&_s);
  string_delete(&_e);
}

TEST(Datetime, TimeTToTm) {
  struct timeval tv = {1393548153, 704907};
  struct tm t;

  t = time_t_to_tm(&tv.tv_sec, true);

  EXPECT_LEQ(114, t.tm_year);
  EXPECT_LEQ(1, t.tm_mon);
  EXPECT_LEQ(28, t.tm_mday);
  EXPECT_LEQ(0, t.tm_hour);
  EXPECT_LEQ(42, t.tm_min);
  EXPECT_LEQ(33, t.tm_sec);
}

FINISH
