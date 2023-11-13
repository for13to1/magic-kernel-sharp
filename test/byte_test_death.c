#include "byte_test_death.h"

START_DEATH

TEST(Byte, Diff) {
  EXPECT_DEATH(
      byte_encode_diff(-1.01),
      "Diff (-1.01) not in range [-1, +1]");
  EXPECT_DEATH(
      byte_encode_diff(+1.01),
      "Diff (1.01) not in range [-1, +1]");
}

FINISH_DEATH
