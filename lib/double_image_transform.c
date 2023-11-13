#include "double_image_transform.h"
#include <math.h>
#include <float.h>

#define PI_ 3.14159265358979323846264338
#define PI_ON_180_ (PI_ / 180)
#define MIN_ 100000
#define EPS_1_ 1e-4
#define EPS_2_ 1e-10

typedef struct {
  double x;
  double y;
} XY_;

typedef struct {
  bool mightReduce;
  byte numChannels;
  ulong outHeight;
  ulong *ios;
  XY_ **ins;
  XY_ **ks;
  DoubleImageResampler *resampler;
  DoubleImage *this;
} Pass_;

static void transform_(Parallel *par);


DoubleImage *double_image_new_transform(
    DoubleImage **p_this,
    DoubleImage *other,
    DoubleImageTransformer *transformer,
    double *background,
    ulong margin,
    bool might_reduce,
    byte mks_a,
    byte mks_v) {
  byte num_channels;
  ulong e;
  ulong in_height;
  ulong in_i;
  ulong in_j;
  ulong in_width;
  ulong io;
  ulong jo;
  ulong min_parallel;
  ulong num_in_edge_pixels;
  ulong out_height;
  ulong out_width;
  ulong *_in_edge_is = null;
  ulong *_in_edge_js = null;
  ulong *_ios = null;
  long i_out;
  long j_out;
  long min_i_out;
  long max_i_out;
  long min_j_out;
  long max_j_out;
  double dx_sq;
  double dx_sq_x;
  double dx_sq_y;
  double dx_xm;
  double dx_xp;
  double dx_ym;
  double dx_yp;
  double dy_sq;
  double dy_sq_x;
  double dy_sq_y;
  double dy_xm;
  double dy_xp;
  double dy_ym;
  double dy_yp;
  double k_x;
  double k_y;
  double max_recip_k_x;
  double max_recip_k_y;
  double max_x_out;
  double max_y_out;
  double min_x_out;
  double min_y_out;
  double x;
  double xc;
  double y;
  double yc;
  Pass_ pass;
  XY_ **_ins = null;
  XY_ **_ks = null;
  DoubleImageResampler *_resampler = null;
  DoubleImage *this;

  in_width = other->width;
  in_height = other->height;

  double_image_resampler_new(
      &_resampler,
      other,
      background,
      might_reduce,
      mks_a,
      mks_v);

  if (in_width == 1 && in_height == 1) {
    num_in_edge_pixels = 1;
  } else if (in_width == 1) {
    num_in_edge_pixels = in_height;
  } else if (in_height == 1) {
    num_in_edge_pixels = in_width;
  } else {
    num_in_edge_pixels = 2 * (in_width + in_height - 2);
  }

  new(&_in_edge_is, num_in_edge_pixels);
  new(&_in_edge_js, num_in_edge_pixels);

  for (e = 0, in_i = 0; in_i < in_width; in_i++) {
    _in_edge_is[e] = in_i;
    _in_edge_js[e] = 0;
    e++;

    if (in_height != 1) {
      _in_edge_is[e] = in_i;
      _in_edge_js[e] = in_height - 1;
      e++;
    }
  }

  for (in_j = 1; in_j < in_height - 1; in_j++) {
    _in_edge_is[e] = 0;
    _in_edge_js[e] = in_j;
    e++;

    if (in_width != 1) {
      _in_edge_is[e] = in_width - 1;
      _in_edge_js[e] = in_j;
      e++;
    }
  }

  if (e != num_in_edge_pixels) {
    throw("Impossible");
  }

  min_x_out = DBL_MAX;
  max_x_out = -DBL_MAX;
  min_y_out = DBL_MAX;
  max_y_out = -DBL_MAX;

  for (e = 0; e < num_in_edge_pixels; e++) {
    x = _in_edge_is[e];
    y = _in_edge_js[e];
    double_image_transformer_in_to_out(transformer, &x, &y);
    min_x_out = minimum(min_x_out, x);
    max_x_out = maximum(max_x_out, x);
    min_y_out = minimum(min_y_out, y);
    max_y_out = maximum(max_y_out, y);
    double_image_transformer_out_to_in(transformer, &x, &y);

    if (fabs(x - _in_edge_is[e]) > EPS_2_
        || fabs(y - _in_edge_js[e]) > EPS_2_) {
      throw("Impossible: Inconsistency in transforms");
    }
  }

  deletev(&_in_edge_js);
  deletev(&_in_edge_is);

  min_i_out = (long)floor(min_x_out + EPS_1_) - (long)margin;
  max_i_out = (long)ceil(max_x_out - EPS_1_) + (long)margin;
  min_j_out = (long)floor(min_y_out + EPS_1_) - (long)margin;
  max_j_out = (long)ceil(max_y_out - EPS_1_) + (long)margin;

  out_width = max_i_out - min_i_out + 1;
  out_height = max_j_out - min_j_out + 1;

  num_channels = _resampler->numChannels;

  double_image_new(p_this, out_width, out_height, num_channels);
  this = *p_this;

  newm(&_ins, out_width, out_height);

  for (io = 0; io < out_width; io++) {
    for (jo = 0; jo < out_height; jo++) {
      i_out = (long)io + min_i_out;
      j_out = (long)jo + min_j_out;
      x = i_out;
      y = j_out;
      double_image_transformer_out_to_in(transformer, &x, &y);
      _ins[io][jo].x = x;
      _ins[io][jo].y = y;
      double_image_transformer_in_to_out(transformer, &x, &y);

      if (fabs(x - i_out) > EPS_2_ || fabs(y - j_out) > EPS_2_) {
        throw("Impossible: Inconsistency in transforms: "
            "%.16g %ld %.16g %ld",
            x,
            i_out,
            y,
            j_out);
      }
    }
  }

  max_recip_k_x = 1;
  max_recip_k_y = 1;

  if (might_reduce) {
    newm(&_ks, out_width, out_height);

    for (io = 0; io < out_width; io++) {
      for (jo = 0; jo < out_height; jo++) {
        dx_xm = dx_xp = dx_ym = dx_yp = dy_xm = dy_xp = dy_ym = dy_yp = 0;

        xc = _ins[io][jo].x;
        yc = _ins[io][jo].y;

        if (io > 0) {
          dx_xm = xc - _ins[io - 1][jo].x;
          dy_xm = yc - _ins[io - 1][jo].y;
        }

        if (io < out_width - 1) {
          dx_xp = _ins[io + 1][jo].x - xc;
          dy_xp = _ins[io + 1][jo].y - yc;
        }

        if (jo > 0) {
          dx_ym = xc - _ins[io][jo - 1].x;
          dy_ym = yc - _ins[io][jo - 1].y;
        }

        if (jo < out_height - 1) {
          dx_yp = _ins[io][jo + 1].x - xc;
          dy_yp = _ins[io][jo + 1].y - yc;
        }

        dx_sq_x = maximum(dx_xm * dx_xm, dx_xp * dx_xp);
        dx_sq_y = maximum(dx_ym * dx_ym, dx_yp * dx_yp);
        dy_sq_x = maximum(dy_xm * dy_xm, dy_xp * dy_xp);
        dy_sq_y = maximum(dy_ym * dy_ym, dy_yp * dy_yp);

        dx_sq = dx_sq_x + dx_sq_y;
        dy_sq = dy_sq_x + dy_sq_y;

        k_x = 1 / sqrt(dx_sq);
        k_y = 1 / sqrt(dy_sq);

        max_recip_k_x = maximum(max_recip_k_x, 1 / k_x);
        max_recip_k_y = maximum(max_recip_k_y, 1 / k_y);

        _ks[io][jo].x = k_x;
        _ks[io][jo].y = k_y;
      }
    }
  }

  new(&_ios, out_width);

  for (io = 0; io < out_width; io++) {
    _ios[io] = io;
  }

  shuffle(_ios, out_width);

  pass.numChannels = num_channels;
  pass.mightReduce = might_reduce;
  pass.outHeight = out_height;
  pass.resampler = _resampler;
  pass.ins = _ins;
  pass.ks = _ks;
  pass.ios = _ios;
  pass.this = this;

  min_parallel = MIN_
    / (ulong)ceil(out_height * num_channels * max_recip_k_x * max_recip_k_y);
  parallel_for(out_width, min_parallel, transform_, &pass);

  deletev(&_ios);
  double_image_resampler_delete(&_resampler);
  deletemv(&_ks, out_width);
  deletemv(&_ins, out_width);

  return this;
}

DoubleImage *double_image_new_rotate_resize_shift(
    DoubleImage **p_this,
    DoubleImage *other,
    double degrees,
    double scale_factor,
    double x_shift,
    double y_shift,
    double *background,
    ulong margin,
    byte mks_a,
    byte mks_v) {
  DoubleImageTransformerShiftInfo shift_1;
  DoubleImageTransformerShiftInfo shift_2;
  DoubleImageTransformer *transformer = null;

  shift_1.xShift = -((double)other->width - 1) / 2;
  shift_1.yShift = -((double)other->height - 1) / 2;

  shift_2.xShift = -shift_1.xShift + x_shift;
  shift_2.yShift = -shift_1.yShift + y_shift;

  double_image_transformer_new(&transformer);
  double_image_transformer_add_shift(transformer, &shift_1);
  double_image_transformer_add_rotation(transformer, &degrees);
  double_image_transformer_add_resize(transformer, &scale_factor);
  double_image_transformer_add_shift(transformer, &shift_2);

  double_image_new_transform(
      p_this,
      other,
      transformer,
      background,
      margin,
      scale_factor < 1,
      mks_a,
      mks_v);

  double_image_transformer_delete(&transformer);

  return *p_this;
}

DoubleImage *double_image_new_change_perspective(
    DoubleImage **p_this,
    DoubleImage *other,
    double field_of_view_degrees,
    double dov_shift_pixels_x,
    double dov_shift_pixels_y,
    double rotation_degrees,
    double scale_factor,
    double x_shift,
    double y_shift,
    double *background,
    ulong margin,
    byte mks_a,
    byte mks_v) {
  ulong d;
  double c;
  double r;
  double rotation_1;
  double rotation_2;
  double theta;
  DoubleImageTransformerShiftInfo shift_1;
  DoubleImageTransformerShiftInfo shift_2;
  DoubleImageTransformerPerspectiveInfo pers;
  DoubleImageTransformer *transformer = null;

  shift_1.xShift = -((double)other->width - 1) / 2;
  shift_1.yShift = -((double)other->height - 1) / 2;

  shift_2.xShift = -shift_1.xShift + x_shift;
  shift_2.yShift = -shift_1.yShift + y_shift;

  d = maximum(other->width, other->height);

  c = 2 * tan(field_of_view_degrees / 2 * PI_ON_180_) / d;

  r = sqrt(dov_shift_pixels_x * dov_shift_pixels_x
      + dov_shift_pixels_y * dov_shift_pixels_y);
  theta = atan2(dov_shift_pixels_y, dov_shift_pixels_x) / PI_ON_180_;

  rotation_1 = -theta;
  rotation_2 = theta + rotation_degrees;

  pers.cSquared = c * c;
  pers.x0 = r;

  double_image_transformer_new(&transformer);
  double_image_transformer_add_shift(transformer, &shift_1);
  double_image_transformer_add_rotation(transformer, &rotation_1);
  double_image_transformer_add_perspective_change(transformer, &pers);
  double_image_transformer_add_rotation(transformer, &rotation_2);
  double_image_transformer_add_resize(transformer, &scale_factor);
  double_image_transformer_add_shift(transformer, &shift_2);

  double_image_new_transform(
      p_this,
      other,
      transformer,
      background,
      margin,
      true,
      mks_a,
      mks_v);

  double_image_transformer_delete(&transformer);

  return *p_this;
}


static void transform_(Parallel *par) {
  byte c;
  ulong a;
  ulong io;
  ulong jo;
  double values[4];
  XY_ in;
  XY_ k;
  Pass_ *pass;
  DoubleImage *this;

  pass = par->x;

  this = pass->this;

  k.x = 1;
  k.y = 1;

  for (a = par->start; a < par->end; a++) {
    io = pass->ios[a];

    for (jo = 0; jo < pass->outHeight; jo++) {
      in = pass->ins[io][jo];

      if (pass->mightReduce) {
        k = pass->ks[io][jo];
      }

      double_image_resampler_get(pass->resampler, in.x, in.y, k.x, k.y, values);

      for (c = 0; c < pass->numChannels; c++) {
        this->_channels[c][io][jo] = values[c];
      }
    }
  }
}
