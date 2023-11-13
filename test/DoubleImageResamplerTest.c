#include "DoubleImageResamplerTest.h"

START

TEST(DoubleImageResampler, Simple) {
  double background;
  double value;
  DoubleImage *_di = null;
  DoubleImageResampler *_dii = null;

  double_image_new_y(&_di, 2, 2);
  _di->y[0][0] = 0.1;
  _di->y[1][0] = 0.9;
  _di->y[0][1] = 0.6;
  _di->y[1][1] = 0.3;
  background = 0.05;

  /* Nearest-neighbor, no reduction.
   */
  double_image_resampler_new(&_dii, _di, &background, false, 1, 0);

  /* Snap to nearest pixels.
   */
  double_image_resampler_get(_dii, 0.4, 0.4, 1, 1, &value);
  EXPECT_NEAR(0.1, value, 1e-15);
  double_image_resampler_get(_dii, 0.6, 0.6, 2, 3, &value);
  EXPECT_NEAR(0.3, value, 1e-15);
  double_image_resampler_get(_dii, -0.4, 0.6, 1, 1.5, &value);
  EXPECT_NEAR(0.6, value, 1e-15);
  double_image_resampler_get(_dii, 1.4, -0.4, 1.6, 1, &value);
  EXPECT_NEAR(0.9, value, 1e-15);

  double_image_resampler_delete(&_dii);

  /* Linear interpolation, with reduction.
   */
  double_image_resampler_new(&_dii, _di, &background, true, 2, 0);

  /* On pixel centers, no reduction.
   */
  double_image_resampler_get(_dii, 0, 0, 1, 1, &value);
  EXPECT_NEAR(0.1, value, 1e-15);
  double_image_resampler_get(_dii, 1, 0, 1.1, 2, &value);
  EXPECT_NEAR(0.9, value, 1e-15);
  double_image_resampler_get(_dii, 0, 1, 1, 2.2, &value);
  EXPECT_NEAR(0.6, value, 1e-15);
  double_image_resampler_get(_dii, 1, 1, 1.7, 1, &value);
  EXPECT_NEAR(0.3, value, 1e-15);

  /* On pixel centers, with reduction.
   */
  double_image_resampler_get(_dii, 0, 0, 0.5, 1, &value);
  EXPECT_NEAR(0.2875, value, 1e-15);
  double_image_resampler_get(_dii, 1, 0, 1, 0.5, &value);
  EXPECT_NEAR(0.5375, value, 1e-15);
  double_image_resampler_get(_dii, 0, 1, 0.5, 0.5, &value);
  EXPECT_NEAR(0.278125, value, 1e-15);
  double_image_resampler_get(_dii, 1, 1, 0.25, 0.5, &value);
  EXPECT_NEAR(0.188281, value, 1e-6);

  double_image_resampler_delete(&_dii);

  double_image_delete(&_di);
}

TEST(DoubleImageResampler, Print) {
  char *_s = null;
  DoubleImage *_di = null;
  DoubleImageResampler *_dir = null;

  double_image_new_y(&_di, 1, 1);
  double_image_resampler_new(&_dir, _di, null, true, 1, 0);
  aprint(&_s, 0, double_image_resampler, _dir);
  EXPECT_STREQ(
      "[DoubleImageResampler]\n. Might be reduced: true\n. Interpolator:\n"
      ". [DoubleImageInterpolator]\n. . [Image being interpolated]\n"
      ". . . Number of channels: 1\n. . . Width: 1 pixel\n"
      ". . . Height: 1 pixel\n. . [/Image being interpolated]\n"
      ". . [MKS parameters]\n. . . v: 0\n. . . a: 1\n. . [/MKS parameters]\n"
      ". . [Background values]\n. . . Channel 0: 0\n. . [/Background values]\n"
      ". [/DoubleImageInterpolator]\n[/DoubleImageResampler]\n",
      _s);
  string_delete(&_s);

  double_image_resampler_delete(&_dir);
  double_image_delete(&_di);
}

FINISH
