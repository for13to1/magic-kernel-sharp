#include "IeeeDoubleTestDeath.h"

START_DEATH

TEST(IeeeDouble, SignificantBits) {
  EXPECT_DEATH(
      ieee_double_significant_bits(ieee_double_subnormal()),
      "Cannot get number of significant bits for subnormal double "
      "4.940 656 458 412 465 e-324");
  EXPECT_DEATH(
      ieee_double_significant_bits(ieee_double_positive_infinity()),
      "Cannot get number of significant bits for infinite or NaN double inf");
  EXPECT_DEATH(
      ieee_double_significant_bits(ieee_double_negative_infinity()),
      "Cannot get number of significant bits for infinite or NaN double -inf");
  EXPECT_DEATH(
      ieee_double_significant_bits(ieee_double_nan()),
      "Cannot get number of significant bits for infinite or NaN double nan");
}

FINISH_DEATH
