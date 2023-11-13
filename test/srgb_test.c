#include "srgb_test.h"

START

TEST(Srgb, ByteGamma) {
  ushort u;

  for (u = 0; u < 256; u++) {
    EXPECT_ULEQ(
        u,
        srgb_linear_to_byte_gamma(srgb_byte_gamma_to_linear((byte)u)));
  }
}

TEST(Srgb, DoubleGamma) {
  ushort u;
  double d;

  for (u = 0; u < 1000; u++) {
    d = u / 1000.0;
    EXPECT_NEAR(
        d,
        srgb_double_linear_to_gamma(srgb_double_gamma_to_linear(d)),
        2e-16);
  }
}

FINISH
