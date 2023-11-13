#include "gcd_test.h"

START

TEST(GCD, Simple) {
  EXPECT_ULEQ(0, gcd(0, 0));

  EXPECT_ULEQ(1, gcd(1, 0));
  EXPECT_ULEQ(1, gcd(0, 1));

  EXPECT_ULEQ(2, gcd(2, 0));
  EXPECT_ULEQ(2, gcd(0, 2));

  EXPECT_ULEQ(1, gcd(1, 1));
  EXPECT_ULEQ(2, gcd(2, 2));
  EXPECT_ULEQ(3, gcd(3, 3));
  EXPECT_ULEQ(4, gcd(4, 4));
  EXPECT_ULEQ(5, gcd(5, 5));

  EXPECT_ULEQ(1, gcd(2, 1));
  EXPECT_ULEQ(1, gcd(1, 2));
  EXPECT_ULEQ(1, gcd(3, 1));
  EXPECT_ULEQ(1, gcd(1, 3));
  EXPECT_ULEQ(1, gcd(4, 1));
  EXPECT_ULEQ(1, gcd(1, 4));
  EXPECT_ULEQ(1, gcd(5, 1));
  EXPECT_ULEQ(1, gcd(1, 5));

  EXPECT_ULEQ(1, gcd(3, 2));
  EXPECT_ULEQ(1, gcd(2, 3));
  EXPECT_ULEQ(2, gcd(4, 2));
  EXPECT_ULEQ(2, gcd(2, 4));
  EXPECT_ULEQ(1, gcd(5, 2));
  EXPECT_ULEQ(1, gcd(2, 5));
  EXPECT_ULEQ(2, gcd(6, 2));
  EXPECT_ULEQ(2, gcd(2, 6));

  EXPECT_ULEQ(1, gcd(4, 3));
  EXPECT_ULEQ(1, gcd(5, 3));
  EXPECT_ULEQ(3, gcd(6, 3));

  EXPECT_ULEQ(8, gcd(24, 40));
  EXPECT_ULEQ(8, gcd(40, 24));
}

FINISH
