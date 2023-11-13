#include "DoubleImageInterpolatorTestDeath.h"

START_DEATH

TEST(DoubleImageInterpolator, New) {
  DoubleImage *_di = null;
  DoubleImageInterpolator *_dii = null;

  double_image_new_y(&_di, 1, 1);
  EXPECT_DEATH(
      double_image_interpolator_new(&_dii, _di, null, 0, 1),
      "MKS a (0) invalid: must be in [1, 6]");
  double_image_interpolator_delete(&_dii);
  double_image_delete(&_di);

  double_image_new_y(&_di, 1, 1);
  EXPECT_DEATH(
      double_image_interpolator_new(&_dii, _di, null, 7, 1),
      "MKS a (7) invalid: must be in [1, 6]");
  double_image_interpolator_delete(&_dii);
  double_image_delete(&_di);

  double_image_new_y(&_di, 1, 1);
  EXPECT_DEATH(
      double_image_interpolator_new(&_dii, _di, null, 2, 1),
      "MKS v (1) invalid for a = 2: must be zero");
  double_image_interpolator_delete(&_dii);
  double_image_delete(&_di);

  double_image_new_y(&_di, 1, 1);
  EXPECT_DEATH(
      double_image_interpolator_new(&_dii, _di, null, 1, 1),
      "MKS v (1) invalid for a = 1: must be zero");
  double_image_interpolator_delete(&_dii);
  double_image_delete(&_di);

  double_image_new_y(&_di, 1, 1);
  EXPECT_DEATH(
      double_image_interpolator_new(&_dii, _di, null, 3, 0),
      "v (0) invalid for a = 3: must be 1, 3, or 7");
  double_image_interpolator_delete(&_dii);
  double_image_delete(&_di);

  double_image_new_y(&_di, 1, 1);
  EXPECT_DEATH(
      double_image_interpolator_new(&_dii, _di, null, 3, 2),
      "v (2) invalid for a = 3: must be 1, 3, or 7");
  double_image_interpolator_delete(&_dii);
  double_image_delete(&_di);

  double_image_new_y(&_di, 1, 1);
  EXPECT_DEATH(
      double_image_interpolator_new(&_dii, _di, null, 6, 8),
      "v (8) invalid for a = 6: must be in [1, 7]");
  double_image_interpolator_delete(&_dii);
  double_image_delete(&_di);
}

FINISH_DEATH
