#include "DoubleImageTransformer.h"
#include <math.h>

#define PI_ 3.14159265358979323846264338
#define PI_ON_180_ (PI_ / 180)

static void rotate_in_to_out_(double *x, double *y, void *p);

static void rotate_out_to_in_(double *x, double *y, void *p);

static void resize_in_to_out_(double *x, double *y, void *p);

static void resize_out_to_in_(double *x, double *y, void *p);

static void shift_in_to_out_(double *x, double *y, void *p);

static void shift_out_to_in_(double *x, double *y, void *p);

static void perspective_in_to_out_(double *x, double *y, void *p);

static void perspective_out_to_in_(double *x, double *y, void *p);


void double_image_transformer_new(DoubleImageTransformer **p_this) {
  DoubleImageTransformer *this;

  this = newo(p_this);

  new(&this->_inToOuts, 0);
  new(&this->_outToIns, 0);
  new(&this->_ps, 0);
}

void double_image_transformer_delete(DoubleImageTransformer **p_this) {
  DoubleImageTransformer *this;

  if ((this = *p_this) == null) {
    return;
  }

  deletev(&this->_ps);
  deletev(&this->_outToIns);
  deletev(&this->_inToOuts);

  deleteo(p_this);
}

void double_image_transformer_add(
    DoubleImageTransformer *this,
    void (*in_to_out)(double *x, double *y, void *p),
    void (*out_to_in)(double *x, double *y, void *p),
    void *p) {
  append_copy(&this->_inToOuts, this->n, in_to_out);
  this->n--;
  append_copy(&this->_outToIns, this->n, out_to_in);
  this->n--;
  append_copy(&this->_ps, this->n, p);
}

void double_image_transformer_add_rotation(
    DoubleImageTransformer *this,
    double *degrees_clockwise) {
  double_image_transformer_add(
      this,
      rotate_in_to_out_,
      rotate_out_to_in_,
      degrees_clockwise);
}

void double_image_transformer_add_resize(
    DoubleImageTransformer *this,
    double *scale_factor) {
  if (scale_factor == 0) {
    throw("Scale factor cannot be zero");
  }

  double_image_transformer_add(
      this,
      resize_in_to_out_,
      resize_out_to_in_,
      scale_factor);
}

void double_image_transformer_add_shift(
    DoubleImageTransformer *this,
    DoubleImageTransformerShiftInfo *info) {
  double_image_transformer_add(
      this,
      shift_in_to_out_,
      shift_out_to_in_,
      info);
}

void double_image_transformer_add_perspective_change(
    DoubleImageTransformer *this,
    DoubleImageTransformerPerspectiveInfo *info) {
  double_image_transformer_add(
      this,
      perspective_in_to_out_,
      perspective_out_to_in_,
      info);
}

void double_image_transformer_in_to_out(
    DoubleImageTransformer *this,
    double *x,
    double *y) {
  ulong u;

  for (u = 0; u < this->n; u++) {
    this->_inToOuts[u](x, y, this->_ps[u]);
  }
}

void double_image_transformer_out_to_in(
    DoubleImageTransformer *this,
    double *x,
    double *y) {
  ulong u;
  ulong v;

  for (v = 0; v < this->n; v++) {
    u = this->n - 1 - v;
    this->_outToIns[u](x, y, this->_ps[u]);
  }
}


static void rotate_in_to_out_(double *x, double *y, void *p) {
  double c;
  double degrees;
  double lx;
  double ly;
  double s;

  degrees = *(double *)p;

  c = cos(PI_ON_180_ * degrees);
  s = sin(PI_ON_180_ * degrees);

  lx = *x;
  ly = *y;

  *x = lx * c - ly * s;
  *y = lx * s + ly * c;
}

static void rotate_out_to_in_(double *x, double *y, void *p) {
  double c;
  double degrees;
  double lx;
  double ly;
  double s;

  degrees = *(double *)p;

  c = cos(PI_ON_180_ * degrees);
  s = sin(PI_ON_180_ * degrees);

  lx = *x;
  ly = *y;

  *x = lx * c + ly * s;
  *y = -lx * s + ly * c;

}

static void resize_in_to_out_(double *x, double *y, void *p) {
  double k;

  k = *(double *)p;

  *x *= k;
  *y *= k;
}

static void resize_out_to_in_(double *x, double *y, void *p) {
  double k;

  k = *(double *)p;

  *x /= k;
  *y /= k;
}

static void shift_in_to_out_(double *x, double *y, void *p) {
  DoubleImageTransformerShiftInfo *info;

  info = p;

  *x += info->xShift;
  *y += info->yShift;
}

static void shift_out_to_in_(double *x, double *y, void *p) {
  DoubleImageTransformerShiftInfo *info;

  info = p;

  *x -= info->xShift;
  *y -= info->yShift;
}

static void perspective_in_to_out_(double *x, double *y, void *p) {
  double c_squared;
  double csq_x0;
  double one_plus_csq_x0_xin;
  double one_plus_csq_x0sq;
  double x_0;
  DoubleImageTransformerPerspectiveInfo *info;

  info = p;

  c_squared = info->cSquared;
  x_0 = info->x0;
  csq_x0 = c_squared * x_0;
  one_plus_csq_x0_xin = 1 + csq_x0 * *x;
  one_plus_csq_x0sq = 1 + csq_x0 * x_0;

  *x = (*x - x_0) / one_plus_csq_x0_xin;
  *y *= sqrt(one_plus_csq_x0sq) / one_plus_csq_x0_xin;
}

static void perspective_out_to_in_(double *x, double *y, void *p) {
  DoubleImageTransformerPerspectiveInfo *old_info;
  DoubleImageTransformerPerspectiveInfo info;

  old_info = p;
  info.cSquared = old_info->cSquared;
  info.x0 = -old_info->x0;

  perspective_in_to_out_(x, y, &info);
}
