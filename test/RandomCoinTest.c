#include "RandomCoinTest.h"
#include <math.h>

#define N_ 20000000
#define SIGMAS_ 4

START

TEST(RandomCoin, Bulk) {
  ulong k;
  ulong u;
  double f;
  double mean;
  double stdev;
  double z;
  RandomCoin *_coin = null;

  random_coin_new(&_coin);

  for (k = 0, u = 0; u < N_; u++) {
    if (random_coin_flip(_coin)) {
      k++;
    }
  }

  f = (double)k / N_;

  mean = 0.5;
  stdev = 0.5 / sqrt(N_);
  z = (f - mean) / stdev;

  EXPECT_TRUE(fabs(z) < SIGMAS_);

  random_coin_delete(&_coin);
}

FINISH
