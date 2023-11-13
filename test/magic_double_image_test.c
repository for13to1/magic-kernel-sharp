/* We don't need to test too much here, because the components are already
 * tested in magic_test, DoubleImageTest, and double_image_kernel_test. This is
 * just to make sure it's hooked up.
 */
#include "magic_double_image_test.h"

START

TEST(MagicDoubleImage, Resize) {
  bool in;
  bool in_i;
  bool in_j;
  ulong i;
  ulong j;
  DoubleImage *_din = null;
  DoubleImage *_dout = null;

  double_image_new_rgba(&_din, 5, 5);
  _din->r[2][2] = 0.5;
  _din->g[2][2] = 0.75;
  _din->b[2][2] = 0.25;
  _din->a[2][2] = 0.1;

  double_image_new_resize(
      &_dout,
      _din,
      11,
      11,
      2,
      MAGIC_RESIZER_NEAREST_NEIGHBOR,
      0,
      0);

  for (i = 0; i < 11; i++) {
    in_i = i == 4 || i == 5;

    for (j = 0; j < 11; j++) {
      in_j = j == 4 || j == 5;
      in = in_i && in_j;
      EXPECT_DEQ(in ? 0.5 : 0, _dout->r[i][j]);
      EXPECT_DEQ(in ? 0.75 : 0, _dout->g[i][j]);
      EXPECT_DEQ(in ? 0.25 : 0, _dout->b[i][j]);
      EXPECT_DEQ(in ? 0.1 : 1, _dout->a[i][j]);
    }
  }

  double_image_delete(&_dout);
  double_image_delete(&_din);
}

TEST(MagicDoubleImage, Blur) {
  ulong i;
  ulong j;
  double weight;
  double weights[] = {0, 0.125, 0.75, 0.125, 0};
  DoubleImage *_din = null;
  DoubleImage *_dout = null;

  double_image_new_rgba(&_din, 5, 5);
  _din->r[2][2] = 0.5;
  _din->g[2][2] = 0.75;
  _din->b[2][2] = 0.25;
  _din->a[2][2] = 0.1;

  double_image_new_blur(&_dout, _din, 1);

  for (i = 0; i < 5; i++) {
    for (j = 0; j < 5; j++) {
      weight = weights[i] * weights[j];
      EXPECT_DEQ(0.5 * weight, _dout->r[i][j]);
      EXPECT_DEQ(0.75 * weight, _dout->g[i][j]);
      EXPECT_DEQ(0.25 * weight, _dout->b[i][j]);
      EXPECT_NEAR(1 - 0.9 * weight, _dout->a[i][j], 2e-16);
    }
  }

  double_image_delete(&_dout);
  double_image_delete(&_din);
}

TEST(MagicDoubleImage, Sharpen) {
  ulong i;
  ulong j;
  double weight;
  double weights[] = {0, -0.25, 1.5, -0.25, 0};
  DoubleImage *_din = null;
  DoubleImage *_dout = null;

  double_image_new_rgba(&_din, 5, 5);
  _din->r[2][2] = 0.5;
  _din->g[2][2] = 0.75;
  _din->b[2][2] = 0.25;
  _din->a[2][2] = 0.1;

  double_image_new_sharpen(&_dout, _din, 1);

  for (i = 0; i < 5; i++) {
    for (j = 0; j < 5; j++) {
      weight = weights[i] * weights[j];
      EXPECT_DEQ(0.5 * weight, _dout->r[i][j]);
      EXPECT_DEQ(0.75 * weight, _dout->g[i][j]);
      EXPECT_DEQ(0.25 * weight, _dout->b[i][j]);
      EXPECT_NEAR(1 - 0.9 * weight, _dout->a[i][j], 3e-16);
    }
  }

  double_image_delete(&_dout);
  double_image_delete(&_din);
}

FINISH
