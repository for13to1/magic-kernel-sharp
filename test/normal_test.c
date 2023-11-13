#include "normal_test.h"

START

TEST(Normal, Normal) {
  EXPECT_NEAR(0.398942, normal(0), 1e-6);

  EXPECT_NEAR(0.241971, normal(+1), 1e-6);
  EXPECT_NEAR(0.241971, normal(-1), 1e-6);

  EXPECT_NEAR(0.053991, normal(+2), 1e-6);
  EXPECT_NEAR(0.053991, normal(-2), 1e-6);

  EXPECT_NEAR(0.00443185, normal(+3), 1e-8);
  EXPECT_NEAR(0.00443185, normal(-3), 1e-8);
}

TEST(Normal, NormInv) {
  EXPECT_NEAR(-3.09023224, norminv(0.001), 1e-7);
  EXPECT_NEAR(-2.32634787, norminv(0.01), 1e-8);
  EXPECT_NEAR(-1.95996398, norminv(0.025), 1e-8);
  EXPECT_NEAR(-1.6448536279, norminv(0.05), 1e-9);
  EXPECT_NEAR(-0.5244005119, norminv(0.3), 1e-9);
  EXPECT_NEAR(-0.2533471033, norminv(0.4), 1e-9);
  EXPECT_DEQ(0, norminv(0.5));
  EXPECT_NEAR(0.2533471033, norminv(0.6), 1e-9);
  EXPECT_NEAR(0.5244005119, norminv(0.7), 1e-9);
  EXPECT_NEAR(1.6448536279, norminv(0.95), 1e-9);
  EXPECT_NEAR(1.95996398, norminv(0.975), 1e-8);
  EXPECT_NEAR(2.32634787, norminv(0.99), 1e-8);
  EXPECT_NEAR(3.09023224, norminv(0.999), 1e-7);
}

TEST(Normal, ErfInv) {
  EXPECT_NEAR(-2.18512422, erfinv(-0.998), 1e-8);
  EXPECT_NEAR(-1.64497636, erfinv(-0.98), 1e-8);
  EXPECT_NEAR(-1.38590382, erfinv(-0.95), 1e-8);
  EXPECT_NEAR(-1.16308715, erfinv(-0.9), 1e-8);
  EXPECT_NEAR(-0.37080716, erfinv(-0.4), 1e-8);
  EXPECT_NEAR(-0.17914346, erfinv(-0.2), 1e-8);
  EXPECT_DEQ(0, erfinv(0));
  EXPECT_NEAR(0.17914346, erfinv(0.2), 1e-8);
  EXPECT_NEAR(0.37080716, erfinv(0.4), 1e-8);
  EXPECT_NEAR(1.16308715, erfinv(0.9), 1e-8);
  EXPECT_NEAR(1.38590382, erfinv(0.95), 1e-8);
  EXPECT_NEAR(1.64497636, erfinv(0.98), 1e-8);
  EXPECT_NEAR(2.18512422, erfinv(0.998), 1e-8);
}

FINISH
