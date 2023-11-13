#include "DoubleImageInterpolator.h"
#include <math.h>

void double_image_interpolator_new(
    DoubleImageInterpolator **p_this,
    DoubleImage *image,
    double *background,
    byte mks_a,
    byte mks_v) {
  byte c;
  byte num_channels;
  ulong base_height;
  ulong base_width;
  ulong height;
  ulong width;
  DoubleImage *_uncropped = null;
  DoubleImageInterpolator *this;

  this = newo(p_this);

  this->mksA = mks_a;
  this->mksV = mks_v;
  width = this->width = image->width;
  height = this->height = image->height;
  num_channels = this->numChannels = image->numChannels;

  new(&this->_background, 4);

  for (c = 0; c < 4; c++) {
    if (background == null || num_channels <= c) {
      this->_background[c] = c == 3 ? 1 : 0;
    } else {
      this->_background[c] = background[c];
    }
  }

  base_width = this->baseWidth = width + mks_v * 2;
  base_height = this->baseHeight = height + mks_v * 2;

  if (mks_a == 0 || mks_a > 6) {
    throw("MKS a (%u) invalid: must be in [1, 6]", mks_a);
  }

  if (mks_a < 3) {
    if (mks_v != 0) {
      throw("MKS v (%u) invalid for a = %u: must be zero", mks_v, mks_a);
    }

    double_image_new_uncrop(
        &this->_base,
        image,
        0,
        base_width,
        0,
        base_height,
        this->_background);
  } else {
    double_image_new_uncrop(
        &_uncropped,
        image,
        -(long)mks_v,
        base_width,
        -(long)mks_v,
        base_height,
        this->_background);

    double_image_new_magic_sharpen(
        &this->_base,
        _uncropped,
        mks_a,
        mks_v);
    double_image_delete(&_uncropped);
  }
}

void double_image_interpolator_delete(DoubleImageInterpolator **p_this) {
  DoubleImageInterpolator *this;

  if ((this = *p_this) == null) {
    return;
  }

  double_image_delete(&this->_base);
  deletev(&this->_background);

  deleteo(p_this);
}

void double_image_interpolator_print(
    DoubleImageInterpolator *this,
    Printer *p) {
  byte c;

  sprint(p, "DoubleImageInterpolator");

  sprint(p, "Image being interpolated");
  rprint(p, "Number of channels: %u", this->numChannels);
  rprint(p, "Width: %u pixel%s", this->width, plural_s(this->width));
  rprint(p, "Height: %u pixel%s", this->width, plural_s(this->height));
  fprint(p);

  sprint(p, "MKS parameters");
  rprint(p, "v: %u", this->mksV);
  rprint(p, "a: %u", this->mksA);
  fprint(p);

  sprint(p, "Background values");

  for (c = 0; c < this->numChannels; c++) {
    rprint(p, "Channel %u: %.16g", c, this->_background[c]);
  }

  fprint(p);
  fprint(p);
}

void double_image_interpolator_get(
    DoubleImageInterpolator *this,
    double x,
    double y,
    double *values) {
  byte c;
  byte mks_a;
  byte mks_v;
  byte num_channels;
  ulong height;
  ulong width;
  long bi;
  long bj;
  long i;
  long j;
  long max_i;
  long max_j;
  long min_i;
  long min_j;
  double half_support;
  double k;
  double kx;
  double ky;
  double v;
  double sum_k;

  mks_a = this->mksA;
  mks_v = this->mksV;
  width = this->baseWidth;
  height = this->baseHeight;
  num_channels = this->numChannels;

  half_support = 0.5 * mks_a;

  min_i = (long)floor(x - half_support);
  max_i = (long)ceil(x + half_support);
  min_j = (long)floor(y - half_support);
  max_j = (long)ceil(y + half_support);

  for (c = 0; c < num_channels; c++) {
    values[c] = 0;
  }

  sum_k = 0;

  for (i = min_i; i <= max_i; i++) {
    kx = magic_kernel(mks_a, i - x);

    if (kx == 0) {
      continue;
    }

    for (j = min_j; j <= max_j; j++) {
      ky = magic_kernel(mks_a, j - y);

      if (ky == 0) {
        continue;
      }

      k = kx * ky;
      bi = i + mks_v;
      bj = j + mks_v;

      for (c = 0; c < num_channels; c++) {
        if (bi < 0 || bi >= width || bj < 0 || bj >= height) {
          v = this->_background[c];
        } else {
          v = this->_base->_channels[c][bi][bj];
        }

        values[c] += v * k;
      }

      sum_k += k;
    }
  }

  if (fabs(sum_k - 1) > 1e-10) {
    throw("Impossible: %.16g (%.16g)", sum_k, sum_k - 1);
  }
}
