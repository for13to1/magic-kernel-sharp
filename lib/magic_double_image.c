#include "magic_double_image.h"

void double_image_new_resize(
    DoubleImage **p_this,
    DoubleImage *other,
    ulong width,
    ulong height,
    double k,
    char *method,
    byte a,
    byte v) {
  byte num_kernel_pairs;
  Kernel **_x_kernels = null;
  Kernel **_y_kernels = null;

  kernels_new_resize(
      &_x_kernels,
      other->width,
      width,
      false,
      k,
      method,
      a,
      v,
      &num_kernel_pairs);

  kernels_new_resize(
      &_y_kernels,
      other->height,
      height,
      false,
      k,
      method,
      a,
      v,
      null);

  double_image_new_apply_multiple_xy_kernels(
    p_this,
    other,
    num_kernel_pairs,
    _x_kernels,
    _y_kernels);

  delete(&_x_kernels, num_kernel_pairs, kernel_delete);
  delete(&_y_kernels, num_kernel_pairs, kernel_delete);
}

void double_image_new_blur(
    DoubleImage **p_this,
    DoubleImage *other,
    double blur) {
  Kernel *_x_kernel = null;
  Kernel *_y_kernel = null;

  kernel_new_blur(&_x_kernel, other->width, false, blur);
  kernel_new_blur(&_y_kernel, other->height, false, blur);

  double_image_new_apply_xy_kernels(p_this, other, _x_kernel, _y_kernel);

  kernel_delete(&_y_kernel);
  kernel_delete(&_x_kernel);
}

void double_image_new_sharpen(
    DoubleImage **p_this,
    DoubleImage *other,
    double sharpening) {
  Kernel *_x_kernel = null;
  Kernel *_y_kernel = null;

  kernel_new_sharpen(&_x_kernel, other->width, false, sharpening);
  kernel_new_sharpen(&_y_kernel, other->height, false, sharpening);

  double_image_new_apply_xy_kernels(p_this, other, _x_kernel, _y_kernel);

  kernel_delete(&_y_kernel);
  kernel_delete(&_x_kernel);
}

void double_image_new_magic_sharpen(
    DoubleImage **p_this,
    DoubleImage *other,
    byte a,
    byte v) {
  byte num_kernel_pairs;
  Kernel **_x_kernels = null;
  Kernel **_y_kernels = null;

  kernels_new_magic_sharpen(
      &_x_kernels,
      other->width,
      false,
      a,
      v,
      &num_kernel_pairs);

  kernels_new_magic_sharpen(
      &_y_kernels,
      other->height,
      false,
      a,
      v,
      null);

  double_image_new_apply_multiple_xy_kernels(
    p_this,
    other,
    num_kernel_pairs,
    _x_kernels,
    _y_kernels);

  delete(&_x_kernels, num_kernel_pairs, kernel_delete);
  delete(&_y_kernels, num_kernel_pairs, kernel_delete);
}
