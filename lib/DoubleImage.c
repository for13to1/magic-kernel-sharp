#include "DoubleImage.h"
#include <float.h>
#include <math.h>

#define MIN_ 100000

typedef struct {
  ulong height;
  double **a;
} MakeOpaquePass_;

typedef struct {
  byte c;
  ulong xOffset;
  ulong yOffset;
  ulong newHeight;
  DoubleImage *other;
  DoubleImage *this;
} CropPass_;

typedef struct {
  byte c;
  long xOffset;
  long yOffset;
  ulong newHeight;
  double *background;
  DoubleImage *other;
  DoubleImage *this;
} UncropPass_;

typedef struct {
  bool quadrature;
  bool subtract;
  byte c;
  DoubleImage *a;
  DoubleImage *b;
  DoubleImage *this;
} AddSubtractPass_;

typedef struct {
  byte c;
  ulong height;
  DoubleImage *other;
  DoubleImage *this;
} TransposePass_;

typedef struct {
  bool transparent;
  ulong height;
  double **a;
} TransparentPass_;

typedef struct {
  bool abs;
  ulong height;
  double max;
  Lock lock;
  DoubleImage *this;
} MaxRgbPass_;

static DoubleImage *new_(
    DoubleImage **p_this,
    ulong width,
    ulong height,
    byte num_channels);

static void add_subtract_(
    DoubleImage **p_this,
    DoubleImage *a,
    DoubleImage *b,
    bool subtract,
    bool quadrature);

static void make_opaque_(Parallel *par);

static void crop_(Parallel *par);

static void uncrop_(Parallel *par);

static void add_subtract_parallel_(Parallel *par);

static void transpose_(Parallel *par);

static void transparent_(Parallel *par);

static void max_rgb_(Parallel *par);


void double_image_new(
    DoubleImage **p_this,
    ulong width,
    ulong height,
    byte num_channels) {
  switch (num_channels) {
    case 1:
      double_image_new_y(p_this, width, height);
      return;
    case 2:
      double_image_new_ya(p_this, width, height);
      return;
    case 3:
      double_image_new_rgb(p_this, width, height);
      return;
    case 4:
      double_image_new_rgba(p_this, width, height);
      return;
    default:
      throw("Number of channels (%u) must be 1, 2, 3, or 4", num_channels);
  }
}

void double_image_new_rgba(DoubleImage **p_this, ulong width, ulong height) {
  DoubleImage *this;

  this = new_(p_this, width, height, 4);

  this->r = this->_channels[0];
  this->g = this->_channels[1];
  this->b = this->_channels[2];
  this->a = this->_channels[3];

  double_image_make_opaque(this);
}

void double_image_new_rgb(DoubleImage **p_this, ulong width, ulong height) {
  DoubleImage *this;

  this = new_(p_this, width, height, 3);

  this->r = this->_channels[0];
  this->g = this->_channels[1];
  this->b = this->_channels[2];
}

void double_image_new_ya(DoubleImage **p_this, ulong width, ulong height) {
  DoubleImage *this;

  this = new_(p_this, width, height, 2);

  this->y = this->_channels[0];
  this->a = this->_channels[1];

  double_image_make_opaque(this);
}

void double_image_new_y(DoubleImage **p_this, ulong width, ulong height) {
  DoubleImage *this;

  this = new_(p_this, width, height, 1);

  this->y = this->_channels[0];
}

byte double_images_new_split_channels(
    DoubleImage ***p_this,
    DoubleImage *other) {
  bool gamma;
  byte c;
  byte num_channels;
  ulong width;
  ulong height;
  double **channel;
  DoubleImage *di;
  DoubleImage **this;

  num_channels = other->numChannels;

  if (num_channels == 1) {
    throw("Number of channels cannot be 1");
  }

  width = other->width;
  height = other->height;
  gamma = other->gamma;

  this = new(p_this, num_channels);

  for (c = 0; c < num_channels; c++) {
    double_image_new_y(&this[c], width, height);
    di = this[c];
    di->gamma = gamma;
    channel = di->_channels[0];
    copy_valuesm(other->_channels[c], channel, width, height);
  }

  return num_channels;
}

void double_image_new_from_channels(
    DoubleImage **p_this,
    DoubleImage **channels,
    byte num_channels) {
  bool gamma;
  byte c;
  ulong width;
  ulong height;
  DoubleImage *this;

  if (num_channels == 1) {
    throw("Number of channels cannot be 1");
  }

  width = channels[0]->width;
  height = channels[0]->height;
  gamma = channels[0]->gamma;

  switch (num_channels) {
    case 2:
      double_image_new_ya(p_this, width, height);
      break;
    case 3:
      double_image_new_rgb(p_this, width, height);
      break;
    case 4:
      double_image_new_rgba(p_this, width, height);
      break;
    default:
      throw("Number of channels (%u) must be 2, 3, or 4", num_channels);
  }

  this = *p_this;

  this->gamma = gamma;

  for (c = 0; c < num_channels; c++) {
    copy_valuesm(channels[c]->_channels[0], this->_channels[c], width, height);
  }
}

void double_images_new_split_alpha(
    DoubleImage **p_alpha,
    DoubleImage **p_non_alpha,
    DoubleImage *other) {
  bool gamma;
  byte c;
  byte num_channels;
  ulong height;
  ulong width;
  DoubleImage *alpha;
  DoubleImage *non_alpha;

  num_channels = other->numChannels;

  if (num_channels % 2 != 0) {
    throw("Can only split off alpha if an alpha channel exists");
  }

  width = other->width;
  height = other->height;
  gamma = other->gamma;

  double_image_new_y(p_alpha, width, height);
  alpha = *p_alpha;
  alpha->gamma = gamma;

  double_image_new(p_non_alpha, width, height, num_channels - 1);
  non_alpha = *p_non_alpha;
  non_alpha->gamma = gamma;

  copy_valuesm(other->a, alpha->y, width, height);

  for (c = 0; c < num_channels - 1; c++) {
    copy_valuesm(other->_channels[c], non_alpha->_channels[c], width, height);
  }
}

void double_image_new_add_alpha(
    DoubleImage **p_this,
    DoubleImage *alpha,
    DoubleImage *non_alpha) {
  bool gamma;
  byte c;
  byte num_channels;
  ulong height;
  ulong width;
  DoubleImage *this;

  num_channels = non_alpha->numChannels + 1;

  if (num_channels % 2 != 0) {
    throw("Can only add alpha if no alpha channel exists");
  }

  if (alpha->numChannels != 1) {
    throw("Alpha channel image must be opaque grayscale");
  }

  width = non_alpha->width;
  height = non_alpha->height;
  gamma = non_alpha->gamma;

  if (alpha->width != width) {
    throw("Alpha width (%lu) not equal to non-alpha width (%lu)",
        alpha->width,
        width);
  }

  if (alpha->height != height) {
    throw("Alpha height (%lu) not equal to non-alpha height (%lu)",
        alpha->height,
        height);
  }

  if (alpha->gamma != gamma) {
    throw("Alpha gamma flag (%s) not equal to non-alpha gamma flag (%s)",
        bool_string(alpha->gamma),
        bool_string(gamma));
  }

  double_image_new(p_this, width, height, num_channels);
  this = *p_this;
  this->gamma = gamma;

  for (c = 0; c < num_channels - 1; c++) {
    copy_valuesm(non_alpha->_channels[c], this->_channels[c], width, height);
  }

  copy_valuesm(alpha->y, this->a, width, height);
}

void double_image_new_copy(DoubleImage **p_this, DoubleImage *other) {
  byte c;
  byte num_channels;
  ulong height;
  ulong width;
  DoubleImage *this;

  num_channels = other->numChannels;
  width = other->width;
  height = other->height;
  double_image_new(p_this, width, height, num_channels);
  this = *p_this;

  this->gamma = other->gamma;

  for (c = 0; c < num_channels; c++) {
    copy_valuesm(other->_channels[c], this->_channels[c], width, height);
  }
}

void double_image_new_transpose(DoubleImage **p_this, DoubleImage *other) {
  byte c;
  byte num_channels;
  ulong height;
  ulong width;
  TransposePass_ pass;
  DoubleImage *this;

  num_channels = other->numChannels;
  width = other->height;
  height = other->width;
  double_image_new(p_this, width, height, num_channels);
  this = *p_this;

  this->gamma = other->gamma;
  pass.this = this;
  pass.other = other;
  pass.height = height;

  for (c = 0; c < num_channels; c++) {
    pass.c = c;
    parallel_for(width, MIN_ / height, transpose_, &pass);
  }
}

void double_image_new_crop(
    DoubleImage **p_this,
    DoubleImage *other,
    ulong x_offset,
    ulong new_width,
    ulong y_offset,
    ulong new_height) {
  byte c;
  byte num_channels;
  ulong old_width;
  ulong old_height;
  CropPass_ pass;
  DoubleImage *this;

  old_width = other->width;
  old_height = other->height;

  if (x_offset + new_width > old_width) {
    throw("x offset (%lu) plus new width (%lu) greater than old width (%lu)",
        x_offset,
        new_width,
        old_width);
  }

  if (y_offset + new_height > old_height) {
    throw("y offset (%lu) plus new height (%lu) greater than old height (%lu)",
        y_offset,
        new_height,
        old_height);
  }

  num_channels = other->numChannels;
  double_image_new(p_this, new_width, new_height, num_channels);
  this = *p_this;

  this->gamma = other->gamma;

  pass.this = this;
  pass.other = other;
  pass.xOffset = x_offset;
  pass.yOffset = y_offset;
  pass.newHeight = new_height;

  for (c = 0; c < num_channels; c++) {
    pass.c = c;
    parallel_for(new_width, MIN_ / new_height, crop_, &pass);
  }
}

void double_image_new_uncrop(
    DoubleImage **p_this,
    DoubleImage *other,
    long x_offset,
    ulong new_width,
    long y_offset,
    ulong new_height,
    double *background) {
  bool alpha;
  bool grayscale;
  bool old_alpha;
  bool old_grayscale;
  byte c;
  byte num_channels;
  UncropPass_ pass;
  DoubleImage *this;

  old_grayscale = double_image_is_grayscale(other);
  old_alpha = double_image_has_alpha(other);

  alpha = old_alpha || background[3] != 1;
  grayscale = old_grayscale
    && background[1] == background[0]
    && background[2] == background[1]
    && !alpha;

  num_channels = alpha ? 4 : grayscale ? 1 : 3;

  double_image_new(p_this, new_width, new_height, num_channels);
  this = *p_this;

  this->gamma = other->gamma;

  pass.this = this;
  pass.other = other;
  pass.xOffset = x_offset;
  pass.yOffset = y_offset;
  pass.newHeight = new_height;
  pass.background = background;

  for (c = 0; c < num_channels; c++) {
    pass.c = c;
    parallel_for(new_width, MIN_ / new_height, uncrop_, &pass);
  }
}

void double_image_new_add(
    DoubleImage **p_this,
    DoubleImage *a,
    DoubleImage *b) {
  add_subtract_(p_this, a, b, false, false);
}

void double_image_new_subtract(
    DoubleImage **p_this,
    DoubleImage *a,
    DoubleImage *b) {
  add_subtract_(p_this, a, b, true, false);
}

void double_image_new_add_quadrature(
    DoubleImage **p_this,
    DoubleImage *a,
    DoubleImage *b) {
  add_subtract_(p_this, a, b, false, true);
}

void double_image_delete(DoubleImage **p_this) {
  DoubleImage *this;

  if ((this = *p_this) == null) {
    return;
  }

  deletet3v(&this->_channels, this->numChannels, this->width);

  deleteo(p_this);
}

void double_image_print(DoubleImage *this, Printer *p) {
  byte c;
  ulong ii;
  ulong j;

  sprint(p, "DoubleImage");
  rprint(p, "Width: %lu", this->width);
  rprint(p, "Height: %lu", this->height);
  rprint(p, "Number of channels: %u", this->numChannels);

  sprint(p, "Contents");

  for (c = 0; c < this->numChannels; c++) {
    sprint(p, "Channel %u", c);

    for (j = 0; j < this->height; j++) {
      sprint(p, "Row %lu", j);

      for (ii = 0; ii < this->width; ii++) {
        rprint(p, "[%lu] %.16g", ii, this->_channels[c][ii][j]);
      }

      fprint(p);
    }

    fprint(p);
  }

  fprint(p);
  fprint(p);
}

bool double_image_has_alpha(DoubleImage *this) {
  return this->numChannels % 2 == 0;
}

bool double_image_is_grayscale(DoubleImage *this) {
  return this->numChannels < 3;
}

bool double_image_contains_transparency(DoubleImage *this) {
  TransparentPass_ pass;

  if (!double_image_has_alpha(this)) {
    return false;
  }

  pass.a = this->a;
  pass.height = this->height;
  pass.transparent = false;
  parallel_for(this->width, MIN_ / this->height, transparent_, &pass);

  return pass.transparent;
}

double double_image_max_rgb(DoubleImage *this) {
  MaxRgbPass_ pass;

  if (this->numChannels < 3) {
    throw("Can only apply to RGB or RGBA images");
  }

  pass.this = this;
  pass.height = this->height;
  pass.max = -DBL_MAX;
  pass.abs = false;
  lock_initialize(&pass.lock);
  parallel_for(this->width, MIN_ / this->height, max_rgb_, &pass);

  return pass.max;
}

double double_image_max_abs_rgb(DoubleImage *this) {
  MaxRgbPass_ pass;

  if (this->numChannels < 3) {
    throw("Can only apply to RGB or RGBA images");
  }

  pass.this = this;
  pass.height = this->height;
  pass.max = -DBL_MAX;
  pass.abs = true;
  lock_initialize(&pass.lock);
  parallel_for(this->width, MIN_ / this->height, max_rgb_, &pass);

  return pass.max;
}



/*
up to here for parallelizing
 */



double double_image_mean_abs_rgb(DoubleImage *this) {
  byte c;
  ulong i;
  ulong j;
  double sum;

  for (sum = 0, c = 0; c < minimum(3, this->numChannels); c++) {
    for (i = 0; i < this->width; i++) {
      for (j = 0; j < this->height; j++) {
        sum += fabs(this->_channels[c][i][j]);
      }
    }
  }

  return sum / (minimum(3, this->numChannels) * this->width * this->height);
}

void double_image_make_opaque(DoubleImage *this) {
  MakeOpaquePass_ pass;

  if (!double_image_has_alpha(this)) {
    throw("Image does not have alpha channel");
  }

  pass.a = this->a;
  pass.height = this->height;
  parallel_for(this->width, MIN_ / this->height, make_opaque_, &pass);
}

void double_image_bound(DoubleImage *this, double min, double max) {
  byte c;
  ulong i;
  ulong j;
  double d;

  for (c = 0; c < this->numChannels; c++) {
    for (i = 0; i < this->width; i++) {
      for (j = 0; j < this->height; j++) {
        d = this->_channels[c][i][j];

        if (d < min) {
          this->_channels[c][i][j] = min;
        } else if (d > max) {
          this->_channels[c][i][j] = max;
        }
      }
    }
  }
}

void double_image_auto_scale(DoubleImage *this) {
  byte c;
  ulong i;
  ulong j;
  double d;
  double max;
  double min;
  double range;
  double scale;

  if (double_image_has_alpha(this)) {
    throw("Cannot auto-scale when the image has an alpha channel");
  }

  min = DBL_MAX;
  max = -DBL_MAX;

  for (c = 0; c < this->numChannels; c++) {
    for (i = 0; i < this->width; i++) {
      for (j = 0; j < this->height; j++) {
        d = this->_channels[c][i][j];

        min = minimum(min, d);
        max = maximum(max, d);
      }
    }
  }

  if (min == 0 && max == 1) {
    return;
  }

  range = max - min;

  if (range == 0) {
    for (c = 0; c < this->numChannels; c++) {
      for (i = 0; i < this->width; i++) {
        for (j = 0; j < this->height; j++) {
          this->_channels[c][i][j] = 0.5;
        }
      }
    }

    return;
  }

  scale = 1 / range;

  for (c = 0; c < this->numChannels; c++) {
    for (i = 0; i < this->width; i++) {
      for (j = 0; j < this->height; j++) {
        d = this->_channels[c][i][j];
        this->_channels[c][i][j] = scale * (d - min);
      }
    }
  }
}

void double_image_scale(DoubleImage *this, double m, double c) {
  byte ch;
  ulong i;
  ulong j;
  double d;

  for (ch = 0; ch < this->numChannels; ch++) {
    for (i = 0; i < this->width; i++) {
      for (j = 0; j < this->height; j++) {
        d = this->_channels[ch][i][j];
        this->_channels[ch][i][j] = m * d + c;
      }
    }
  }
}

void double_image_scale_rgb(DoubleImage *this, double factor) {
  byte c;
  byte num_channels;
  ulong i;
  ulong j;

  num_channels = minimum(this->numChannels, 3);

  for (c = 0; c < num_channels; c++) {
    for (i = 0; i < this->width; i++) {
      for (j = 0; j < this->height; j++) {
        this->_channels[c][i][j] *= factor;
      }
    }
  }
}

void double_image_linear_to_gamma(DoubleImage *this) {
  byte c;
  ulong i;
  ulong j;

  if (this->gamma) {
    throw("Can only call double_image_linear_to_gamma() on linear images");
  }

  for (c = 0; c < minimum(this->numChannels, 3); c++) {
    for (i = 0; i < this->width; i++) {
      for (j = 0; j < this->height; j++) {
        this->_channels[c][i][j]
          = srgb_double_linear_to_gamma(this->_channels[c][i][j]);
      }
    }
  }

  this->gamma = true;
}

void double_image_gamma_to_linear(DoubleImage *this) {
  byte c;
  ulong i;
  ulong j;

  if (!this->gamma) {
    throw("Can only call double_image_gamma_to_linear() on gamma images");
  }

  for (c = 0; c < minimum(this->numChannels, 3); c++) {
    for (i = 0; i < this->width; i++) {
      for (j = 0; j < this->height; j++) {
        this->_channels[c][i][j]
          = srgb_double_gamma_to_linear(this->_channels[c][i][j]);
      }
    }
  }

  this->gamma = false;
}


static DoubleImage *new_(
    DoubleImage **p_this,
    ulong width,
    ulong height,
    byte num_channels) {
  DoubleImage *this;

  if (width == 0) {
    throw("Width must be nonzero");
  }

  if (height == 0) {
    throw("Height must be nonzero");
  }

  if (num_channels == 0 || num_channels > 4) {
    throw("Number of channels (%u) must be 1, 2, 3, or 4", num_channels);
  }

  this = newo(p_this);

  this->width = width;
  this->height = height;
  newt3(&this->_channels, this->numChannels = num_channels, width, height);

  return this;
}

static void add_subtract_(
    DoubleImage **p_this,
    DoubleImage *a,
    DoubleImage *b,
    bool subtract,
    bool quadrature) {
  byte c;
  byte num_channels;
  ulong height;
  ulong width;
  AddSubtractPass_ pass;
  DoubleImage *this;

  num_channels = a->numChannels;
  width = a->width;
  height = a->height;

  if (b->numChannels != num_channels) {
    throw("Images have different numbers of channels (%u v %u)",
        num_channels,
        b->numChannels);
  }

  if (b->width != width) {
    throw("Images have different widths (%lu v %lu)", width, b->width);
  }

  if (b->height != height) {
    throw("Images have different heights (%lu v %lu)", height, b->height);
  }

  if (b->gamma != a->gamma) {
    throw("Images have different gamma flags");
  }

  double_image_new(p_this, width, height, num_channels);
  this = *p_this;

  this->gamma = a->gamma;

  pass.this = this;
  pass.a = a;
  pass.b = b;
  pass.quadrature = quadrature;
  pass.subtract = subtract;

  for (c = 0; c < num_channels; c++) {
    pass.c = c;
    parallel_for(width, MIN_ / height, add_subtract_parallel_, &pass);
  }
}


static void make_opaque_(Parallel *par) {
  ulong height;
  ulong i;
  ulong j;
  double **a;
  MakeOpaquePass_ *pass;

  pass = par->x;

  height = pass->height;
  a = pass->a;

  for (i = par->start; i < par->end; i++) {
    for (j = 0; j < height; j++) {
      a[i][j] = 1;
    }
  }
}

static void crop_(Parallel *par) {
  byte c;
  ulong i;
  ulong j;
  ulong old_i;
  ulong old_j;
  CropPass_ *pass;
  DoubleImage *other;
  DoubleImage *this;

  pass = par->x;

  this = pass->this;
  other = pass->other;
  c = pass->c;

  for (i = par->start, old_i = i + pass->xOffset; i < par->end; i++, old_i++) {
    for (j = 0, old_j = pass->yOffset; j < pass->newHeight; j++, old_j++) {
      this->_channels[c][i][j]
        = other->_channels[c][old_i][old_j];
    }
  }
}

static void uncrop_(Parallel *par) {
  byte c;
  byte old_num_channels;
  bool grayscale;
  ulong i;
  ulong j;
  ulong new_height;
  ulong new_width;
  ulong old_height;
  ulong old_width;
  long oi;
  long oj;
  long x_offset;
  long y_offset;
  double v;
  double *background;
  UncropPass_ *pass;
  DoubleImage *other;
  DoubleImage *this;

  pass = par->x;

  this = pass->this;
  other = pass->other;

  old_num_channels = other->numChannels;
  new_width = this->width;
  new_height = this->height;
  old_width = other->width;
  old_height = other->height;
  x_offset = pass->xOffset;
  y_offset = pass->yOffset;
  background = pass->background;
  c = pass->c;
  grayscale = double_image_is_grayscale(this);

  for (i = 0, oi = x_offset; i < new_width; i++, oi++) {
    for (j = 0, oj = y_offset; j < new_height; j++, oj++) {
      if (oi < 0 || oi >= old_width || oj < 0 || oj >= old_height) {
        v = background[c];
      } else {
        if (c >= old_num_channels) {
          if (c == 3 || (grayscale && c == 1)) {
            v = 1;
          } else {
            v = other->_channels[0][oi][oj];
          }
        } else {
          v = other->_channels[c][oi][oj];
        }
      }

      this->_channels[c][i][j] = v;
    }
  }
}

static void add_subtract_parallel_(Parallel *par) {
  bool quadrature;
  bool subtract;
  byte c;
  ulong height;
  ulong i;
  ulong j;
  double da;
  double db;
  double r;
  AddSubtractPass_ *pass;
  DoubleImage *a;
  DoubleImage *b;
  DoubleImage *this;

  pass = par->x;

  this = pass->this;
  a = pass->a;
  b = pass->b;
  c = pass->c;
  quadrature = pass->quadrature;
  subtract = pass->subtract;
  height = this->height;

  for (i = par->start; i < par->end; i++) {
    for (j = 0; j < height; j++) {
      da = a->_channels[c][i][j];
      db = b->_channels[c][i][j];

      if (quadrature) {
        r = sqrt(da * da + db * db);
      } else if (subtract) {
        r = da - db;
      } else {
        r = da + db;
      }

      this->_channels[c][i][j] = r;
    }
  }
}

static void transpose_(Parallel *par) {
  byte c;
  ulong height;
  ulong i;
  ulong j;
  TransposePass_ *pass;
  DoubleImage *other;
  DoubleImage *this;

  pass = par->x;

  this = pass->this;
  other = pass->other;
  height = pass->height;
  c = pass->c;

  for (i = par->start; i < par->end; i++) {
    for (j = 0; j < height; j++) {
      this->_channels[c][i][j] = other->_channels[c][j][i];
    }
  }
}

static void transparent_(Parallel *par) {
  ulong i;
  ulong j;
  ulong height;
  double **a;
  TransparentPass_ *pass;

  pass = par->x;
  height = pass->height;
  a = pass->a;

  for (i = par->start; !pass->transparent && i < par->end; i++) {
    for (j = 0; !pass->transparent && j < height; j++) {
      if (a[i][j] != 1) {
        pass->transparent = true;

        return;
      }
    }
  }
}

static void max_rgb_(Parallel *par) {
  bool abs;
  byte c;
  byte num_channels;
  ulong i;
  ulong j;
  double d;
  double max;
  MaxRgbPass_ *pass;
  DoubleImage *this;

  pass = par->x;

  this = pass->this;
  num_channels = minimum(3, this->numChannels);
  abs = pass->abs;

  for (i = par->start, max = -DBL_MAX; i < par->end; i++) {
    for (c = 0; c < num_channels; c++) {
      for (j = 0; j < this->height; j++) {
        d = this->_channels[c][i][j];

        if (abs) {
          d = fabs(d);
        }

        max = maximum(max, d);
      }
    }
  }

  lock(&pass->lock);
  pass->max = maximum(pass->max, max);
  unlock(&pass->lock);
}
