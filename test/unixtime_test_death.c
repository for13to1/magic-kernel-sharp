#include "unixtime_test_death.h"

START_DEATH

TEST(StringNew, TimeInterval) {
  char *_ = null;

  EXPECT_DEATH(
      string_new_time_interval(&_, -1),
      "Time interval was negative: -1 us");
  EXPECT_DEATH(
      string_new_time_interval(&_, -2),
      "Time interval was negative: -2 us");
  EXPECT_DEATH(
      string_new_time_interval(&_, (ulong)LONG_MAX + 1),
      "Time interval was negative: -9,223,372,036,854,775,808 us");

  string_delete(&_);
}

TEST(StringNew, TimeElapsed) {
  char *_ = null;

  EXPECT_DEATH(
      string_new_time_elapsed(&_, -1, 0),
      "Now (-1 us) is earlier than then (0 us)");
  EXPECT_DEATH(
      string_new_time_elapsed(&_, 0, 1),
      "Now (0 us) is earlier than then (1 us)");
  EXPECT_DEATH(
      string_new_time_elapsed(&_, 1, 2),
      "Now (1 us) is earlier than then (2 us)");

  EXPECT_DEATH(
      string_new_time_elapsed(&_, 12345, 23456),
      "Now (12,345 us) is earlier than then (23,456 us)");

  string_delete(&_);
}

FINISH_DEATH
