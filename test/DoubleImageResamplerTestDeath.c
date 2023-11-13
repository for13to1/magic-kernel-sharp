#include "DoubleImageResamplerTestDeath.h"

START_DEATH

TEST(DoubleImageResampler, Reduce) {
  double value;
  DoubleImage *_di = null;
  DoubleImageResampler *_dir = null;

  double_image_new_y(&_di, 1, 1);
  double_image_resampler_new(&_dir, _di, null, false, 6, 7);
  EXPECT_DEATH(
      double_image_resampler_get(_dir, 0, 0, 0.5, 1.5, &value),
      "At least one component of local k (0.5, 1.5) implies reduction, "
      "which was not specified at construction");
  EXPECT_DEATH(
      double_image_resampler_get(_dir, 0, 0, 1.5, 0.5, &value),
      "At least one component of local k (1.5, 0.5) implies reduction, "
      "which was not specified at construction");
  double_image_resampler_delete(&_dir);
  double_image_delete(&_di);
}

FINISH_DEATH
