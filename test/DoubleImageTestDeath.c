#include "DoubleImageTestDeath.h"

START_DEATH

TEST(DoubleImage, New) {
  DoubleImage *_di = null;

  EXPECT_DEATH(
      double_image_new(&_di, 1, 1, 0),
      "Number of channels (0) must be 1, 2, 3, or 4");
  double_image_delete(&_di);

  EXPECT_DEATH(
      double_image_new(&_di, 1, 1, 5),
      "Number of channels (5) must be 1, 2, 3, or 4");
  double_image_delete(&_di);
}

TEST(DoubleImage, NewRgba) {
  DoubleImage *_di = null;

  EXPECT_DEATH(double_image_new_rgba(&_di, 0, 1), "Width must be nonzero");
  double_image_delete(&_di);

  EXPECT_DEATH(double_image_new_rgba(&_di, 1, 0), "Height must be nonzero");
  double_image_delete(&_di);
}

TEST(DoubleImage, NewRgb) {
  DoubleImage *_di = null;

  EXPECT_DEATH(double_image_new_rgb(&_di, 0, 1), "Width must be nonzero");
  double_image_delete(&_di);

  EXPECT_DEATH(double_image_new_rgb(&_di, 1, 0), "Height must be nonzero");
  double_image_delete(&_di);
}

TEST(DoubleImage, NewGrayscale) {
  DoubleImage *_di = null;

  EXPECT_DEATH(double_image_new_y(&_di, 0, 1), "Width must be nonzero");
  double_image_delete(&_di);

  EXPECT_DEATH(
      double_image_new_y(&_di, 1, 0),
      "Height must be nonzero");
  double_image_delete(&_di);
}

TEST(DoubleImages, NewSplitChannels) {
  DoubleImage *_di = null;
  DoubleImage **_channels = null;

  double_image_new_y(&_di, 1, 1);
  EXPECT_DEATH(
      double_images_new_split_channels(&_channels, _di),
      "Number of channels cannot be 1");
  double_image_delete(&_di);
}

TEST(DoubleImage, NewFromChannels) {
  DoubleImage *_di = null;
  DoubleImage *_gray = null;

  double_image_new_y(&_gray, 1, 1);
  EXPECT_DEATH(
      double_image_new_from_channels(&_di, &_gray, 1),
      "Number of channels cannot be 1");
  double_image_delete(&_gray);
}

TEST(DoubleImages, NewSplitAlpha) {
  DoubleImage *_di = null;
  DoubleImage *_alpha = null;
  DoubleImage *_non_alpha = null;

  double_image_new_y(&_di, 1, 1);
  EXPECT_DEATH(
      double_images_new_split_alpha(&_alpha, &_non_alpha, _di),
      "Can only split off alpha if an alpha channel exists");
  double_image_delete(&_di);
}

TEST(DoubleImages, NewAddAlpha) {
  DoubleImage *_di = null;
  DoubleImage *_alpha = null;
  DoubleImage *_non_alpha = null;

  double_image_new_rgba(&_non_alpha, 1, 1);
  double_image_new_y(&_alpha, 1, 1);
  EXPECT_DEATH(
      double_image_new_add_alpha(&_di, _alpha, _non_alpha),
      "Can only add alpha if no alpha channel exists");
  double_image_delete(&_alpha);
  double_image_delete(&_non_alpha);

  double_image_new_rgb(&_non_alpha, 1, 1);
  double_image_new_rgb(&_alpha, 1, 1);
  EXPECT_DEATH(
      double_image_new_add_alpha(&_di, _alpha, _non_alpha),
      "Alpha channel image must be opaque grayscale");
  double_image_delete(&_alpha);
  double_image_delete(&_non_alpha);

  double_image_new_rgb(&_non_alpha, 1, 1);
  double_image_new_y(&_alpha, 1, 2);
  EXPECT_DEATH(
      double_image_new_add_alpha(&_di, _alpha, _non_alpha),
      "Alpha height (2) not equal to non-alpha height (1)");
  double_image_delete(&_alpha);
  double_image_delete(&_non_alpha);

  double_image_new_rgb(&_non_alpha, 2, 1);
  double_image_new_y(&_alpha, 1, 1);
  EXPECT_DEATH(
      double_image_new_add_alpha(&_di, _alpha, _non_alpha),
      "Alpha width (1) not equal to non-alpha width (2)");
  double_image_delete(&_alpha);
  double_image_delete(&_non_alpha);

  double_image_new_rgb(&_non_alpha, 1, 1);
  double_image_new_y(&_alpha, 1, 1);
  _non_alpha->gamma = true;
  EXPECT_DEATH(
      double_image_new_add_alpha(&_di, _alpha, _non_alpha),
      "Alpha gamma flag (false) not equal to non-alpha gamma flag (true)");
  double_image_delete(&_alpha);
  double_image_delete(&_non_alpha);

  double_image_new_rgb(&_non_alpha, 1, 1);
  double_image_new_y(&_alpha, 1, 1);
  _alpha->gamma = true;
  EXPECT_DEATH(
      double_image_new_add_alpha(&_di, _alpha, _non_alpha),
      "Alpha gamma flag (true) not equal to non-alpha gamma flag (false)");
  double_image_delete(&_alpha);
  double_image_delete(&_non_alpha);
}

TEST(DoubleImage, Crop) {
  DoubleImage *_a = null;
  DoubleImage *_b = null;

  double_image_new_rgb(&_a, 1, 2);

  EXPECT_DEATH(
      double_image_new_crop(&_b, _a, 1, 1, 0, 2),
      "x offset (1) plus new width (1) greater than old width (1)");
  double_image_delete(&_b);

  EXPECT_DEATH(
      double_image_new_crop(&_b, _a, 0, 1, 1, 2),
      "y offset (1) plus new height (2) greater than old height (2)");
  double_image_delete(&_b);

  EXPECT_DEATH(
      double_image_new_crop(&_b, _a, 0, 1, 2, 1),
      "y offset (2) plus new height (1) greater than old height (2)");
  double_image_delete(&_b);

  double_image_delete(&_a);
}

TEST(DoubleImage, MakeOpaque) {
  DoubleImage *_di = null;

  double_image_new_rgb(&_di, 1, 1);
  EXPECT_DEATH(
      double_image_make_opaque(_di),
      "Image does not have alpha channel");
  double_image_delete(&_di);
}

TEST(DoubleImage, Max) {
  DoubleImage *_di = null;

  double_image_new_ya(&_di, 1, 1);
  EXPECT_DEATH(
      double_image_max_rgb(_di),
      "Can only apply to RGB or RGBA images");
  double_image_delete(&_di);

  double_image_new_y(&_di, 1, 1);
  EXPECT_DEATH(
      double_image_max_rgb(_di),
      "Can only apply to RGB or RGBA images");
  double_image_delete(&_di);

  double_image_new_ya(&_di, 1, 1);
  EXPECT_DEATH(
      double_image_max_abs_rgb(_di),
      "Can only apply to RGB or RGBA images");
  double_image_delete(&_di);

  double_image_new_y(&_di, 1, 1);
  EXPECT_DEATH(
      double_image_max_abs_rgb(_di),
      "Can only apply to RGB or RGBA images");
  double_image_delete(&_di);
}

TEST(DoubleImage, LinearToGamma) {
  DoubleImage *_di = null;

  double_image_new_rgb(&_di, 1, 1);
  _di->gamma = true;
  EXPECT_DEATH(
      double_image_linear_to_gamma(_di),
      "Can only call double_image_linear_to_gamma() on linear images");
  double_image_delete(&_di);
}

TEST(DoubleImage, GammaToLinear) {
  DoubleImage *_di = null;

  double_image_new_rgb(&_di, 1, 1);
  EXPECT_DEATH(
      double_image_gamma_to_linear(_di),
      "Can only call double_image_gamma_to_linear() on gamma images");
  double_image_delete(&_di);
}

TEST(DoubleImage, AutoScale) {
  DoubleImage *_di = null;

  double_image_new_rgba(&_di, 1, 1);
  EXPECT_DEATH(
      double_image_auto_scale(_di),
      "Cannot auto-scale when the image has an alpha channel");
  double_image_delete(&_di);
}

TEST(DoubleImage, AddSubtract) {
  DoubleImage *_a = null;
  DoubleImage *_b = null;
  DoubleImage *_c = null;

  double_image_new_rgb(&_a, 1, 2);
  double_image_new_rgba(&_b, 1, 2);
  EXPECT_DEATH(
      double_image_new_add(&_c, _a, _b),
      "Images have different numbers of channels (3 v 4)");
  double_image_delete(&_c);
  EXPECT_DEATH(
      double_image_new_subtract(&_c, _a, _b),
      "Images have different numbers of channels (3 v 4)");
  EXPECT_DEATH(
      double_image_new_add_quadrature(&_c, _a, _b),
      "Images have different numbers of channels (3 v 4)");
  double_image_delete(&_c);
  double_image_delete(&_b);
  double_image_delete(&_a);

  double_image_new_rgb(&_a, 1, 2);
  double_image_new_rgb(&_b, 2, 2);
  EXPECT_DEATH(
      double_image_new_add(&_c, _a, _b),
      "Images have different widths (1 v 2)");
  double_image_delete(&_c);
  EXPECT_DEATH(
      double_image_new_subtract(&_c, _a, _b),
      "Images have different widths (1 v 2)");
  double_image_delete(&_c);
  EXPECT_DEATH(
      double_image_new_add_quadrature(&_c, _a, _b),
      "Images have different widths (1 v 2)");
  double_image_delete(&_c);
  double_image_delete(&_b);
  double_image_delete(&_a);

  double_image_new_rgb(&_a, 2, 3);
  double_image_new_rgb(&_b, 2, 2);
  EXPECT_DEATH(
      double_image_new_add(&_c, _a, _b),
      "Images have different heights (3 v 2)");
  double_image_delete(&_c);
  EXPECT_DEATH(
      double_image_new_subtract(&_c, _a, _b),
      "Images have different heights (3 v 2)");
  double_image_delete(&_c);
  EXPECT_DEATH(
      double_image_new_add_quadrature(&_c, _a, _b),
      "Images have different heights (3 v 2)");
  double_image_delete(&_c);
  double_image_delete(&_b);
  double_image_delete(&_a);
}

FINISH_DEATH
