#include "RandomTestDeath.h"

START_DEATH

TEST(Random, Modulo) {
  Random *_random = null;

  random_new(&_random);
  EXPECT_DEATH(random_modulo(_random, 0), "Modulus cannot be zero");
}

FINISH_DEATH
