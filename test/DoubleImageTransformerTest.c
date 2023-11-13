#include "DoubleImageTransformerTest.h"
#include <math.h>

START

TEST(DoubleImageTransformer, Rotation) {
  double degrees;
  double x;
  double y;
  DoubleImageTransformer *_ = null;

  /* Zero degrees: should be an identity.
   */
  double_image_transformer_new(&_);
  degrees = 0;
  double_image_transformer_add_rotation(_, &degrees);
  x = 1;
  y = 2;
  double_image_transformer_in_to_out(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_out_to_in(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_delete(&_);

  /* 90 degrees.
   */
  double_image_transformer_new(&_);
  degrees = 90;
  double_image_transformer_add_rotation(_, &degrees);
  x = 1;
  y = 2;
  double_image_transformer_in_to_out(_, &x, &y);
  EXPECT_NEAR(-2, x, 1e-15);
  EXPECT_NEAR(1, y, 1e-15);
  double_image_transformer_out_to_in(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_delete(&_);

  /* -30 degrees.
   */
  double_image_transformer_new(&_);
  degrees = -30;
  double_image_transformer_add_rotation(_, &degrees);
  x = 1;
  y = 2;
  double_image_transformer_in_to_out(_, &x, &y);
  EXPECT_NEAR(sqrt(3) / 2 + 1, x, 1e-15);
  EXPECT_NEAR(sqrt(3) - 0.5, y, 1e-15);
  double_image_transformer_out_to_in(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_delete(&_);
}

TEST(DoubleImageTransformer, Resize) {
  double factor;
  double x;
  double y;
  DoubleImageTransformer *_ = null;

  /* Unit factor: should be an identity.
   */
  double_image_transformer_new(&_);
  factor = 1;
  double_image_transformer_add_resize(_, &factor);
  x = 1;
  y = 2;
  double_image_transformer_in_to_out(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_out_to_in(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_delete(&_);

  /* Factor of 3.
   */
  double_image_transformer_new(&_);
  factor = 3;
  double_image_transformer_add_resize(_, &factor);
  x = 1;
  y = 2;
  double_image_transformer_in_to_out(_, &x, &y);
  EXPECT_NEAR(3, x, 1e-15);
  EXPECT_NEAR(6, y, 1e-15);
  double_image_transformer_out_to_in(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_delete(&_);

  /* Factor of -2.5.
   */
  double_image_transformer_new(&_);
  factor = -2.5;
  double_image_transformer_add_resize(_, &factor);
  x = 1;
  y = 2;
  double_image_transformer_in_to_out(_, &x, &y);
  EXPECT_NEAR(-2.5, x, 1e-15);
  EXPECT_NEAR(-5, y, 1e-15);
  double_image_transformer_out_to_in(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_delete(&_);
}

TEST(DoubleImageTransformer, Shift) {
  double x;
  double y;
  DoubleImageTransformerShiftInfo shift;
  DoubleImageTransformer *_ = null;

  /* Zero shift: should be an identity.
   */
  shift.xShift = 0;
  shift.yShift = 0;
  double_image_transformer_new(&_);
  double_image_transformer_add_shift(_, &shift);
  x = 1;
  y = 2;
  double_image_transformer_in_to_out(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_out_to_in(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_delete(&_);

  /* Shift by (-3, +4).
   */
  shift.xShift = -3;
  shift.yShift = 4;
  double_image_transformer_new(&_);
  double_image_transformer_add_shift(_, &shift);
  x = 1;
  y = 2;
  double_image_transformer_in_to_out(_, &x, &y);
  EXPECT_NEAR(-2, x, 1e-15);
  EXPECT_NEAR(6, y, 1e-15);
  double_image_transformer_out_to_in(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_delete(&_);
}

TEST(DoubleImageTransformer, PerspectiveChange) {
  double x;
  double y;
  DoubleImageTransformerPerspectiveInfo pers;
  DoubleImageTransformer *_ = null;

  /* x0 = 0: should be an identity.
   */
  pers.cSquared = 1;
  pers.x0 = 0;
  double_image_transformer_new(&_);
  double_image_transformer_add_perspective_change(_, &pers);
  x = 1;
  y = 2;
  double_image_transformer_in_to_out(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_out_to_in(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_delete(&_);

  /* Shift direction of view to given location.
   */
  pers.cSquared = 1;
  pers.x0 = 1;
  double_image_transformer_new(&_);
  double_image_transformer_add_perspective_change(_, &pers);
  x = 1;
  y = 2;
  double_image_transformer_in_to_out(_, &x, &y);
  EXPECT_NEAR(0, x, 1e-15);
  EXPECT_NEAR(sqrt(2), y, 1e-15);
  double_image_transformer_out_to_in(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_delete(&_);

  /* Shift direction of view to arbitrary position.
   */
  pers.cSquared = 2;
  pers.x0 = 0.5;
  double_image_transformer_new(&_);
  double_image_transformer_add_perspective_change(_, &pers);
  x = 1;
  y = 2;
  double_image_transformer_in_to_out(_, &x, &y);
  EXPECT_NEAR(0.25, x, 1e-15);
  EXPECT_NEAR(sqrt(1.5), y, 1e-15);
  double_image_transformer_out_to_in(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_delete(&_);
}

TEST(DoubleImageTransformer, Compound) {
  double degrees;
  double x;
  double y;
  DoubleImageTransformerShiftInfo shift;
  DoubleImageTransformer *_ = null;

  /* Shift then rotate.
   */
  shift.xShift = 3;
  shift.yShift = 7;
  degrees = 90;
  double_image_transformer_new(&_);
  double_image_transformer_add_shift(_, &shift);
  double_image_transformer_add_rotation(_, &degrees);
  x = 1;
  y = 2;
  double_image_transformer_in_to_out(_, &x, &y);
  EXPECT_NEAR(-9, x, 1e-15);
  EXPECT_NEAR(4, y, 1e-15);
  double_image_transformer_out_to_in(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_delete(&_);

  /* Rotate then shift.
   */
  double_image_transformer_new(&_);
  double_image_transformer_add_rotation(_, &degrees);
  double_image_transformer_add_shift(_, &shift);
  x = 1;
  y = 2;
  double_image_transformer_in_to_out(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(8, y, 1e-15);
  double_image_transformer_out_to_in(_, &x, &y);
  EXPECT_NEAR(1, x, 1e-15);
  EXPECT_NEAR(2, y, 1e-15);
  double_image_transformer_delete(&_);
}

FINISH
