#include "magic_test_death.h"

START_DEATH

TEST(Magic, MagicKernel) {
  EXPECT_DEATH(magic_kernel(0, 0), "a (0) must be in [1, 6]");
  EXPECT_DEATH(magic_kernel(7, 0), "a (7) must be in [1, 6]");
}

TEST(Magic, LanczosKernel) {
  EXPECT_DEATH(lanczos_kernel(0, 0), "a cannot be zero");
}

FINISH_DEATH
