#include "DoubleImageInterpolatorTest.h"

START

TEST(DoubleImageInterpolator, Simple) {
  double background;
  double value;
  DoubleImage *_di = null;
  DoubleImageInterpolator *_dii = null;

  double_image_new_y(&_di, 2, 2);
  _di->y[0][0] = 0.1;
  _di->y[1][0] = 0.9;
  _di->y[0][1] = 0.6;
  _di->y[1][1] = 0.3;
  background = 0.05;

  /* Nearest-neighbor.
   */
  double_image_interpolator_new(&_dii, _di, &background, 1, 0);

  /* Well outside the image.
   */
  double_image_interpolator_get(_dii, -5, -6, &value);
  EXPECT_NEAR(0.05, value, 1e-15);
  double_image_interpolator_get(_dii, 5, 6, &value);
  EXPECT_NEAR(0.05, value, 1e-15);

  /* On pixel centers.
   */
  double_image_interpolator_get(_dii, 0, 0, &value);
  EXPECT_NEAR(0.1, value, 1e-15);
  double_image_interpolator_get(_dii, 1, 0, &value);
  EXPECT_NEAR(0.9, value, 1e-15);
  double_image_interpolator_get(_dii, 0, 1, &value);
  EXPECT_NEAR(0.6, value, 1e-15);
  double_image_interpolator_get(_dii, 1, 1, &value);
  EXPECT_NEAR(0.3, value, 1e-15);

  /* Snap to nearest pixels.
   */
  double_image_interpolator_get(_dii, 0.4, 0.4, &value);
  EXPECT_NEAR(0.1, value, 1e-15);
  double_image_interpolator_get(_dii, 0.6, 0.6, &value);
  EXPECT_NEAR(0.3, value, 1e-15);
  double_image_interpolator_get(_dii, -0.4, 0.6, &value);
  EXPECT_NEAR(0.6, value, 1e-15);
  double_image_interpolator_get(_dii, 1.4, -0.4, &value);
  EXPECT_NEAR(0.9, value, 1e-15);

  /* Outside pixel tiles.
   */
  double_image_interpolator_get(_dii, -0.6, 0.5, &value);
  EXPECT_NEAR(0.05, value, 1e-15);
  double_image_interpolator_get(_dii, 1.6, 0.5, &value);
  EXPECT_NEAR(0.05, value, 1e-15);
  double_image_interpolator_get(_dii, 0.5, -0.6, &value);
  EXPECT_NEAR(0.05, value, 1e-15);
  double_image_interpolator_get(_dii, 0.5, 1.6, &value);
  EXPECT_NEAR(0.05, value, 1e-15);

  double_image_interpolator_delete(&_dii);

  /* Linear interpolation.
   */
  double_image_interpolator_new(&_dii, _di, &background, 2, 0);

  /* Well outside the image.
   */
  double_image_interpolator_get(_dii, -5, -6, &value);
  EXPECT_NEAR(0.05, value, 1e-15);
  double_image_interpolator_get(_dii, 5, 6, &value);
  EXPECT_NEAR(0.05, value, 1e-15);

  /* On pixel centers.
   */
  double_image_interpolator_get(_dii, 0, 0, &value);
  EXPECT_NEAR(0.1, value, 1e-15);
  double_image_interpolator_get(_dii, 1, 0, &value);
  EXPECT_NEAR(0.9, value, 1e-15);
  double_image_interpolator_get(_dii, 0, 1, &value);
  EXPECT_NEAR(0.6, value, 1e-15);
  double_image_interpolator_get(_dii, 1, 1, &value);
  EXPECT_NEAR(0.3, value, 1e-15);

  /* Interpolate pixels.
   */
  double_image_interpolator_get(_dii, 0.5, 0, &value);
  EXPECT_NEAR(0.5, value, 1e-15);
  double_image_interpolator_get(_dii, 0, 0.5, &value);
  EXPECT_NEAR(0.35, value, 1e-15);
  double_image_interpolator_get(_dii, 1, 0.5, &value);
  EXPECT_NEAR(0.6, value, 1e-15);
  double_image_interpolator_get(_dii, 0.5, 1, &value);
  EXPECT_NEAR(0.45, value, 1e-15);
  double_image_interpolator_get(_dii, 0.5, 0.5, &value);
  EXPECT_NEAR(0.475, value, 1e-15);

  /* Outside pixel extent.
   */
  double_image_interpolator_get(_dii, -1.01, 0.5, &value);
  EXPECT_NEAR(0.05, value, 1e-15);
  double_image_interpolator_get(_dii, 2.01, 0.5, &value);
  EXPECT_NEAR(0.05, value, 1e-15);
  double_image_interpolator_get(_dii, 0.5, -1.01, &value);
  EXPECT_NEAR(0.05, value, 1e-15);
  double_image_interpolator_get(_dii, 0.5, 2.01, &value);
  EXPECT_NEAR(0.05, value, 1e-15);

  double_image_interpolator_delete(&_dii);

  /* Full Magic Kernel Sharp (6, 7).
   */
  double_image_interpolator_new(&_dii, _di, &background, 6, 7);

  /* Well outside the image.
   */
  double_image_interpolator_get(_dii, -5, -6, &value);
  EXPECT_NEAR(0.05, value, 1e-15);
  double_image_interpolator_get(_dii, 5, 6, &value);
  EXPECT_NEAR(0.05, value, 1e-15);

  /* On pixel centers. In this case, neighboring pixels do affect the
   * interpolated values.
   */
  double_image_interpolator_get(_dii, 0, 0, &value);
  EXPECT_NEAR(0.100275, value, 1e-7);
  double_image_interpolator_get(_dii, 1, 0, &value);
  EXPECT_NEAR(0.904675, value, 1e-7);
  double_image_interpolator_get(_dii, 0, 1, &value);
  EXPECT_NEAR(0.603025, value, 1e-7);
  double_image_interpolator_get(_dii, 1, 1, &value);
  EXPECT_NEAR(0.301375, value, 1e-7);

  /* Interpolate pixels.
   */
  double_image_interpolator_get(_dii, 0.5, 0, &value);
  EXPECT_NEAR(0.610961, value, 1e-6);
  double_image_interpolator_get(_dii, 0, 0.5, &value);
  EXPECT_NEAR(0.423974, value, 1e-6);
  double_image_interpolator_get(_dii, 1, 0.5, &value);
  EXPECT_NEAR(0.735619, value, 1e-6);
  double_image_interpolator_get(_dii, 0.5, 1, &value);
  EXPECT_NEAR(0.548632, value, 1e-6);
  double_image_interpolator_get(_dii, 0.5, 0.5, &value);
  EXPECT_NEAR(0.706820, value, 1e-6);

  /* Outside pixel extent.
   */
  double_image_interpolator_get(_dii, -10.01, 0.5, &value);
  EXPECT_NEAR(0.05, value, 1e-15);
  double_image_interpolator_get(_dii, 11.01, 0.5, &value);
  EXPECT_NEAR(0.05, value, 1e-15);
  double_image_interpolator_get(_dii, 0.5, -10.01, &value);
  EXPECT_NEAR(0.05, value, 1e-15);
  double_image_interpolator_get(_dii, 0.5, 11.01, &value);
  EXPECT_NEAR(0.05, value, 1e-15);

  double_image_interpolator_delete(&_dii);

  double_image_delete(&_di);
}

TEST(DoubleImageInterpolator, Print) {
  char *_s = null;
  DoubleImage *_di = null;
  DoubleImageInterpolator *_dii = null;

  double_image_new_y(&_di, 1, 1);
  double_image_interpolator_new(&_dii, _di, null, 1, 0);
  aprint(&_s, 0, double_image_interpolator, _dii);
  EXPECT_STREQ(
      "[DoubleImageInterpolator]\n. [Image being interpolated]\n"
      ". . Number of channels: 1\n. . Width: 1 pixel\n. . Height: 1 pixel\n"
      ". [/Image being interpolated]\n. [MKS parameters]\n. . v: 0\n. . a: 1\n"
      ". [/MKS parameters]\n. [Background values]\n. . Channel 0: 0\n"
      ". [/Background values]\n[/DoubleImageInterpolator]\n",
      _s);
  string_delete(&_s);

  double_image_interpolator_delete(&_dii);
  double_image_delete(&_di);
}

FINISH
