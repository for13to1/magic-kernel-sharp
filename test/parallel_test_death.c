#include "parallel_test_death.h"
#include <limits.h>

START_DEATH

TEST(Parallel, ForSpecify) {
  EXPECT_DEATH(
      parallel_for_specify(0, 0, null, null),
      "Cannot specify no threads");
}

FINISH_DEATH
