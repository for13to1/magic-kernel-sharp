#include "unixtime_test.h"

#define BASE_ 12345
#define MS_ 1000UL
#define S_ (MS_ * MS_)
#define M_ (60 * S_)
#define H_ (60 * M_)
#define D_ (24 * H_)
#define Y_ (1461 * D_ /4)

static void test_(char *e, ulong us);


START

TEST(Unixtime, CurrentMicroseconds) {
  /* Just test that they work.
   */
  unixtime_current_us();
  unixtime_current();
}

TEST(StringNew, TimeInterval) {
  char *_ = null;

  CALL(test_("00000", 0));
  CALL(test_("001\xce\xbcs", 1));
  CALL(test_("009\xce\xbcs", 9));
  CALL(test_("010\xce\xbcs", 10));
  CALL(test_("011\xce\xbcs", 11));
  CALL(test_("099\xce\xbcs", 99));
  CALL(test_("100\xce\xbcs", 100));
  CALL(test_("101\xce\xbcs", 101));
  CALL(test_("999\xce\xbcs", 999));
  CALL(test_("1.0ms", MS_));
  CALL(test_("1.0ms", MS_ + 1));
  CALL(test_("1.0ms", MS_ + 99));
  CALL(test_("1.1ms", MS_ + 100));
  CALL(test_("1.1ms", MS_ + 101));
  CALL(test_("9.9ms", 9 * MS_ + 900));
  CALL(test_("9.9ms", 9 * MS_ + 999));
  CALL(test_("010ms", 10 * MS_));
  CALL(test_("010ms", 10 * MS_ + 999));
  CALL(test_("011ms", 11 * MS_));
  CALL(test_("023ms", 23 * MS_ + 456));
  CALL(test_("099ms", 99 * MS_ + 999));
  CALL(test_("100ms", 100 * MS_));
  CALL(test_("100ms", 100 * MS_ + 999));
  CALL(test_("101ms", 101 * MS_));
  CALL(test_("999ms", 999 * MS_ + 999));
  CALL(test_("1.00s", S_));
  CALL(test_("1.00s", S_ + 9 * MS_ + 999));
  CALL(test_("1.01s", S_ + 10 * MS_));
  CALL(test_("1.23s", S_ + 234 * MS_ + 567));
  CALL(test_("9.99s", 9 * S_ + 999 * MS_ + 999));
  CALL(test_("10.0s", 10 * S_));
  CALL(test_("10.0s", 10 * S_ + 99 * MS_ + 999));
  CALL(test_("10.1s", 10 * S_ + 100 * MS_));
  CALL(test_("12.3s", 12 * S_ + 345 * MS_ + 678));
  CALL(test_("59.9s", 59 * S_ + 999 * MS_ + 999));
  CALL(test_("1.00m", M_));
  CALL(test_("1.00m", M_ + 599 * MS_ + 999));
  CALL(test_("1.01m", M_ + 600 * MS_));
  CALL(test_("1.09m", M_ + 5 * S_ + 999 * MS_ + 999));
  CALL(test_("1.10m", M_ + 6 * S_));
  CALL(test_("2.34m", 2 * M_ + 20 * S_ + 456 * MS_ + 789));
  CALL(test_("9.99m", 9 * M_ + 59 * S_ + 999 * MS_ + 999));
  CALL(test_("10.0m", 10 * M_));
  CALL(test_("59.9m", 59 * M_ + 59 * S_ + 999 * MS_ + 999));
  CALL(test_("1.00h", H_));
  CALL(test_("9.99h", 9 * H_ + 59 * M_ + 59 * S_ + 999 * MS_ + 999));
  CALL(test_("10.0h", 10 * H_));
  CALL(test_("23.9h", 23 * H_ + 59 * M_ + 59 * S_));
  CALL(test_("1.00d", D_));
  CALL(test_("0365d", 365 * D_ + 5 * H_ + 59 * M_ + 59 * S_ + 999 * MS_ + 999));
  CALL(test_("1.00y", Y_));
  CALL(test_("12.3y", 12 * Y_ + 110 * D_));
  CALL(test_("0456y", 456 * Y_));
  CALL(test_("7891y", 7891 * Y_));
  string_new_time_interval(&_, 12345 * Y_);
  EXPECT_STREQ("12345y", _);
  string_delete(&_);
}

TEST(StringNew, TimeElapsed) {
  char *_ = null;

  string_new_time_elapsed(&_, BASE_, 0);
  EXPECT_STREQ("00000", _);
  string_delete(&_);

  string_new_time_elapsed(&_, BASE_ + 10, -1);
  EXPECT_STREQ("00000", _);
  string_delete(&_);

  string_new_time_elapsed(&_, BASE_ + 100, -10);
  EXPECT_STREQ("00000", _);
  string_delete(&_);

  string_new_time_elapsed(&_, BASE_ + 12 * MS_ + 345, BASE_);
  EXPECT_STREQ("012ms", _);
  string_delete(&_);
}

FINISH


static void test_(char *e, ulong us) {
  char *_ = null;

  string_new_time_interval(&_, us);
  EXPECT_STREQ(e, _);
  string_delete(&_);
}
