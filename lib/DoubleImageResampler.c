#include "DoubleImageResampler.h"
#include <float.h>
#include <math.h>

void double_image_resampler_new(
    DoubleImageResampler **p_this,
    DoubleImage *image,
    double *background,
    bool might_reduce,
    byte mks_a,
    byte mks_v) {
  byte c;
  byte num_channels;
  ulong height;
  ulong width;
  DoubleImageResampler *this;

  this = newo(p_this);

  this->mightReduce = might_reduce;
  this->mksA = mks_a;
  this->mksV = mks_v;
  width = this->width = image->width;
  height = this->height = image->height;
  this->numChannels = num_channels = image->numChannels;

  new(&this->_background, 4);

  for (c = 0; c < 4; c++) {
    if (background == null || num_channels <= c) {
      this->_background[c] = c == 3 ? 1 : 0;
    } else {
      this->_background[c] = background[c];
    }
  }

  double_image_interpolator_new(
      &this->_interpolator,
      image,
      background,
      mks_a,
      mks_v);

  if (might_reduce) {
    double_image_new_uncrop(
        &this->_image,
        image,
        0,
        width,
        0,
        height,
        this->_background);
  }
}

void double_image_resampler_delete(DoubleImageResampler **p_this) {
  DoubleImageResampler *this;

  if ((this = *p_this) == null) {
    return;
  }

  double_image_interpolator_delete(&this->_interpolator);
  double_image_delete(&this->_image);
  deletev(&this->_background);

  deleteo(p_this);
}

void double_image_resampler_print(DoubleImageResampler *this, Printer *p) {
  sprint(p, "DoubleImageResampler");
  rprint(p, "Might be reduced: %s", bool_string(this->mightReduce));
  rprint(p, "Interpolator:");
  double_image_interpolator_print(this->_interpolator, p);
  fprint(p);
}

void double_image_resampler_get(
    DoubleImageResampler *this,
    double x,
    double y,
    double k_x,
    double k_y,
    double *values) {
  byte c;
  byte mks_a;
  byte mks_v;
  byte num_channels;
  ulong height;
  ulong width;
  long i;
  long j;
  long max_i;
  long max_j;
  long min_i;
  long min_j;
  double half_support_x;
  double half_support_y;
  double w;
  double wx;
  double wy;
  double scale;
  double v;
  double sum_w;

  width = this->width;
  height = this->height;

  if (k_x >= 1 && k_y >= 1) {
    double_image_interpolator_get(this->_interpolator, x, y, values);

    return;
  }

  if (!this->mightReduce) {
    throw("At least one component of local k (%.16g, %.16g) implies reduction, "
        "which was not specified at construction",
        k_x,
        k_y);
  }

  mks_a = this->mksA;
  mks_v = this->mksV;
  num_channels = this->numChannels;

  half_support_x = (0.5 * mks_a + mks_v) / k_x;
  half_support_y = (0.5 * mks_a + mks_v) / k_y;

  min_i = (long)floor(x - half_support_x);
  max_i = (long)ceil(x + half_support_x);
  min_j = (long)floor(y - half_support_y);
  max_j = (long)ceil(y + half_support_y);

  for (c = 0; c < num_channels; c++) {
    values[c] = 0;
  }

  sum_w = 0;

  for (i = min_i; i <= max_i; i++) {
    wx = magic_kernel_sharp_kernel(mks_a, mks_v, k_x * (i - x));

    if (wx == 0) {
      continue;
    }

    for (j = min_j; j <= max_j; j++) {
      wy = magic_kernel_sharp_kernel(mks_a, mks_v, k_y * (j - y));

      if (wy == 0) {
        continue;
      }

      w = wx * wy;
      sum_w += w;
    }
  }

  scale = 1 / sum_w;

  for (c = 0; c < num_channels; c++) {
    values[c] = 0;
  }

  sum_w = 0;

  for (i = min_i; i <= max_i; i++) {
    wx = magic_kernel_sharp_kernel(mks_a, mks_v, k_x * (i - x));

    if (wx == 0) {
      continue;
    }

    for (j = min_j; j <= max_j; j++) {
      wy = magic_kernel_sharp_kernel(mks_a, mks_v, k_y * (j - y));

      if (wy == 0) {
        continue;
      }

      w = scale * wx * wy;

      for (c = 0; c < num_channels; c++) {
        if (i < 0 || i >= width || j < 0 || j >= height) {
          v = this->_background[c];
        } else {
          v = this->_image->_channels[c][i][j];
        }

        values[c] += w * v;
      }

      sum_w += w;
    }
  }

  if (fabs(sum_w - 1) > 1e-10) {
    throw("Impossible: %.16g (%.16g)", sum_w, sum_w - 1);
  }
}
