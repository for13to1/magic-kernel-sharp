#include "double_image_kernel_test_death.h"

START_DEATH

TEST(DoubleImageKernel, NewApplyXYKernels) {
  Kernel *_x_kernel = null;
  Kernel *_y_kernel = null;
  DoubleImage *_din = null;
  DoubleImage *_dout = null;

  double_image_new_rgba(&_din, 2, 3);
  kernel_new_identity(&_x_kernel, 4);
  kernel_new_identity(&_y_kernel, 3);
  EXPECT_DEATH(
      double_image_new_apply_xy_kernels(&_dout, _din, _x_kernel, _y_kernel),
      "X kernel n_in (4) must equal input image width (2)");
  double_image_delete(&_dout);

  kernel_delete(&_y_kernel);
  kernel_delete(&_x_kernel);
  kernel_new_identity(&_x_kernel, 2);
  kernel_new_identity(&_y_kernel, 5);
  EXPECT_DEATH(
      double_image_new_apply_xy_kernels(&_dout, _din, _x_kernel, _y_kernel),
      "Y kernel n_in (5) must equal input image height (3)");
  double_image_delete(&_dout);

  kernel_delete(&_y_kernel);
  kernel_delete(&_x_kernel);
  double_image_delete(&_din);
}

TEST(DoubleImageKernel, NewApplyMultipleXYKernels) {
  EXPECT_DEATH(
      double_image_new_apply_multiple_xy_kernels(
        null,
        null,
        0,
        null,
        null),
      "Number of kernel pairs cannot be zero");
}

FINISH_DEATH
