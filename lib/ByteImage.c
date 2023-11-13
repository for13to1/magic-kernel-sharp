#include "ByteImage.h"

#define MIN_ 100000
#define MIN_CONVERT_ 10000
#define FIX_ROWS_ 6

#undef DEBUG_DITHER_

#ifdef DEBUG_DITHER_
#define debug_(x) (x) /= 64; (x) *= 64;
#else
#define debug_(x)
#endif

typedef struct {
  ByteImagePixel pixel;
  ByteImage *this;
} ClearPass_;

typedef struct {
  bool fix;
  bool ycc;
  ulong height;
  ulong numThreads;
  ulong width;
  ulong *_ends;
  double ***_adds;
  Lock lock;
  DoubleImage *dither;
  ByteImage *this;
} FromDoublePass_;

typedef struct {
  DoubleImage *diff;
  ByteImage *this;
} FromDoubleDiffPass_;

typedef struct {
  ByteImage *other;
  DoubleImage *this;
} DoubleFromBytePass_;

typedef struct {
  ByteImage *other;
  DoubleImage *this;
} DoubleFromByteDiffPass_;

typedef struct {
  bool or;
  ulong height;
  ByteImage *this;
} OrPass_;

static ulong new_(ByteImage **p_this, ulong width, ulong height);

static void from_double_(Parallel *par);

static void from_double_diff_(Parallel *par);

static void double_from_byte_(Parallel *par);

static void double_from_byte_diff_(Parallel *par);

static void transparent_(Parallel *par);

static void grayscale_(Parallel *par);

static void check_(ByteImage *this, ulong i, ulong j);

static ByteImagePixel get_(ByteImage *this, ulong i, ulong j);

static byte get_r_(ByteImage *this, ulong i, ulong j);

static byte get_g_(ByteImage *this, ulong i, ulong j);

static byte get_b_(ByteImage *this, ulong i, ulong j);

static byte get_a_(ByteImage *this, ulong i, ulong j);

static byte get_y_(ByteImage *this, ulong i, ulong j);

static void set_(ByteImage *this, ulong i, ulong j, ByteImagePixel pixel);

static void set_r_(ByteImage *this, ulong i, ulong j, byte r);

static void set_g_(ByteImage *this, ulong i, ulong j, byte g);

static void set_b_(ByteImage *this, ulong i, ulong j, byte b);

static void set_a_(ByteImage *this, ulong i, ulong j, byte a);

static void set_y_(ByteImage *this, ulong i, ulong j, byte a);

static double hue_to_rgb_(double p, double q, double t);

static void make_opaque_(Parallel *par);

static void clear_(Parallel *par);


void byte_image_new(ByteImage **p_this, ulong width, ulong height) {
  new_(p_this, width, height);
  byte_image_make_opaque(*p_this);
}

void byte_image_new_transparent(
    ByteImage **p_this,
    ulong width,
    ulong height) {
  new_(p_this, width, height);
}

void byte_image_new_copy(ByteImage **p_this, ByteImage *other) {
  new_(p_this, other->width, other->height);
  copy_valuesm(other->_pixels, (*p_this)->_pixels, other->height, other->width);
}

void byte_image_new_load(ByteImage **p_this, File *f) {
  bool ycc;
  ulong width;
  ulong height;
  ByteImage *this;

  load_label("[ByteImage]", f);
  load_value(width, f);
  load_value(height, f);
  load_value(ycc, f);
  byte_image_new(p_this, width, height);
  this = *p_this;
  load_valuesm(this->_pixels, this->height, this->width, f);
  load_label("[/ByteImage]", f);
}

void byte_image_new_from_double(ByteImage **p_this, DoubleImage *other) {
  bool ycc;
  byte c;
  byte num_channels;
  ulong end_j;
  ulong height;
  ulong i;
  ulong j;
  ulong last_end;
  ulong ideal_start_j;
  ulong start_j;
  ulong t;
  ulong width;
  double v;
  FromDoublePass_ pass;
  Parallel par;
  DoubleImage *_dither = null;
  ByteImage *this;

  ycc = other->gamma;

  double_image_new_copy(&_dither, other);

  width = _dither->width;
  height = _dither->height;
  num_channels = _dither->numChannels;

  if (ycc && num_channels != 3) {
    throw("Cannot have YCbCr without 3 channels");
  }

  byte_image_new(p_this, width, height);
  this = *p_this;

  pass.this = this;
  pass.dither = _dither;
  pass.ycc = ycc;
  pass.width = width;
  pass.height = height;
  pass.numThreads = 0;
  pass._ends = null;
  pass._adds = null;
  pass.fix = false;
  lock_initialize(&pass.lock);

  parallel_for(height, MIN_CONVERT_ / width, from_double_, &pass);

  for (t = 0, last_end = 0; t < pass.numThreads - 1; t++) {
    ideal_start_j = pass._ends[t];
    start_j = maximum(ideal_start_j, last_end);
    end_j = minimum(start_j + FIX_ROWS_, height);

    for (c = 0; c < num_channels; c++) {
      for (j = start_j; j < end_j; j++) {
        for (i = 0; i < width; i++) {
          v = other->_channels[c][i][j];

          if (j == start_j) {
            v += pass._adds[t - (j == ideal_start_j ? 0 : 1)][c][i];
          }

          _dither->_channels[c][i][j] = v;
        }
      }
    }

    par.t = t;
    par.start = maximum(start_j, last_end);
    par.end = last_end = end_j;
    pass.fix = true;
    par.x = &pass;
    from_double_(&par);
  }

  deletev(&pass._ends);
  deletet3v(&pass._adds, pass.numThreads - 1, _dither->numChannels);

  double_image_delete(&_dither);
}

void byte_image_new_from_double_diff(
    ByteImage **p_this,
    DoubleImage *diff) {
  ulong height;
  ulong width;
  FromDoubleDiffPass_ pass;
  ByteImage *this;

  if (!diff->gamma) {
    throw("Byte differences must be computed in gamma space");
  }

  if (double_image_is_grayscale(diff)) {
    throw("JPEG-Clear does not support grayscale images");
  }

  if (double_image_has_alpha(diff)) {
    throw("JPEG-Clear does not support images with an alpha channel");
  }

  width = diff->width;
  height = diff->height;

  byte_image_new(p_this, width, height);
  this = *p_this;

  pass.this = this;
  pass.diff = diff;
  parallel_for(width, MIN_ / height, from_double_diff_, &pass);
}

void byte_image_delete(ByteImage **p_this) {
  ByteImage *this;

  if ((this = *p_this) == null) {
    return;
  }

  deletemv(&this->_pixels, this->height);
  deleteo(p_this);
}

void byte_image_print(ByteImage *this, Printer *p) {
  ulong i;
  ulong j;
  ByteImagePixel pixel;

  sprint(p, "ByteImage");
  rprint(p, "Width: %lu pixel%s", this->width, plural_s(this->width));
  rprint(p, "Height: %lu pixel%s", this->height, plural_s(this->height));
  rprint(p, "YCC: %s", bool_string(this->ycc));

  sprint(p, "Contents");

  for (j = 0; j < this->height; j++) {
    sprint(p, "Row %lu", j);

    for (i = 0; i < this->width; i++) {
      pixel = get_(this, i, j);
      rprint(p,
          "[%lu]  A: 0x%02x  R: 0x%02x  G: 0x%02x  B: 0x%02x",
          i,
          pixel.a,
          pixel.r,
          pixel.g,
          pixel.b);

    }

    fprint(p);
  }

  fprint(p);
  fprint(p);
}

void byte_image_save(ByteImage *this, File *f) {
  save_label("[ByteImage]", f);
  save_value(this->width, f);
  save_value(this->height, f);
  save_value(this->ycc, f);
  save_valuesm(this->_pixels, this->height, this->width, f);
  save_label("[/ByteImage]", f);
}

bool byte_image_equals(ByteImage *this, ByteImage *other) {
  return other->width == this->width
    && other->height == this->height
    && other->ycc == this->ycc
    && values_equalm(other->_pixels, this->_pixels, this->height, this->width);
}

void double_image_new_from_byte(DoubleImage **p_this, ByteImage *other) {
  bool alpha;
  bool gray;
  bool ycc;
  ulong height;
  ulong width;
  DoubleFromBytePass_ pass;
  DoubleImage *this;

  width = other->width;
  height = other->height;
  ycc = other->ycc;
  alpha = byte_image_contains_transparency(other);
  gray = alpha ? false : byte_image_is_grayscale(other);

  if (alpha) {
    double_image_new_rgba(p_this, width, height);
  } else if (gray) {
    double_image_new_y(p_this, width, height);
  } else {
    double_image_new_rgb(p_this, width, height);
  }

  this = *p_this;

  if (ycc) {
    this->gamma = true;
  }

  pass.this = this;
  pass.other = other;

  parallel_for(width, MIN_CONVERT_ / height, double_from_byte_, &pass);
}

void double_image_new_from_byte_diff(
    DoubleImage **p_this,
    ByteImage *other) {
  ulong height;
  ulong width;
  DoubleFromByteDiffPass_ pass;
  DoubleImage *this;

  width = other->width;
  height = other->height;

  if (byte_image_contains_transparency(other)) {
    throw("impossible");
  }

  double_image_new_rgb(p_this, width, height);
  this = *p_this;

  this->gamma = true;

  pass.this = this;
  pass.other = other;
  parallel_for(width, MIN_ / height, double_from_byte_diff_, &pass);
}

void byte_image_make_opaque(ByteImage *this) {
  parallel_for(this->height, MIN_ / this->width, make_opaque_, this);
}

void byte_image_clear(ByteImage *this, ByteImagePixel pixel) {
  ClearPass_ pass;

  pass.this = this;
  pass.pixel = pixel;
  parallel_for(this->height, MIN_ / this->width, clear_, &pass);
}

ByteImagePixel byte_image_get(ByteImage *this, ulong i, ulong j) {
  check_(this, i, j);
  return get_(this, i, j);
}

byte byte_image_get_r(ByteImage *this, ulong i, ulong j) {
  check_(this, i, j);
  return get_r_(this, i, j);
}

byte byte_image_get_g(ByteImage *this, ulong i, ulong j) {
  check_(this, i, j);
  return get_g_(this, i, j);
}

byte byte_image_get_b(ByteImage *this, ulong i, ulong j) {
  check_(this, i, j);
  return get_b_(this, i, j);
}

byte byte_image_get_a(ByteImage *this, ulong i, ulong j) {
  check_(this, i, j);
  return get_a_(this, i, j);
}

byte byte_image_get_y(ByteImage *this, ulong i, ulong j) {
  check_(this, i, j);
  return get_y_(this, i, j);
}

void byte_image_set(ByteImage *this, ulong i, ulong j, ByteImagePixel pixel) {
  check_(this, i, j);
  set_(this, i, j, pixel);
}

void byte_image_set_r(ByteImage *this, ulong i, ulong j, byte r) {
  check_(this, i, j);
  set_r_(this, i, j, r);
}

void byte_image_set_g(ByteImage *this, ulong i, ulong j, byte g) {
  check_(this, i, j);
  set_g_(this, i, j, g);
}

void byte_image_set_b(ByteImage *this, ulong i, ulong j, byte b) {
  check_(this, i, j);
  set_b_(this, i, j, b);
}

void byte_image_set_a(ByteImage *this, ulong i, ulong j, byte a) {
  check_(this, i, j);
  set_a_(this, i, j, a);
}

void byte_image_set_y(ByteImage *this, ulong i, ulong j, byte a) {
  check_(this, i, j);
  set_y_(this, i, j, a);
}

ByteImageYccaPixel byte_image_get_ycca(ByteImage *this, ulong i, ulong j) {
  ByteImagePixel rgba;
  ByteImageYccaPixel ycca;

  rgba = byte_image_get(this, i, j);
  ycca.y = rgba.r;
  ycca.cb = rgba.g;
  ycca.cr = rgba.b;
  ycca.a = rgba.a;

  return ycca;
}

bool byte_image_contains_transparency(ByteImage *this) {
  OrPass_ pass;

  pass.this = this;
  pass.height = this->height;
  pass.or = false;
  parallel_for(this->width, MIN_ / this->height, transparent_, &pass);

  return pass.or;
}

bool byte_image_is_grayscale(ByteImage *this) {
  OrPass_ pass;

  pass.this = this;
  pass.height = this->height;
  pass.or = false;
  parallel_for(this->width, MIN_ / this->height, grayscale_, &pass);

  return !pass.or;
}

ByteImagePixel byte_image_pixel_from_rgba_hex(uint hex) {
  ByteImagePixel pixel;

  pixel.r = (hex & 0xff000000) >> 24;
  pixel.g = (hex & 0x00ff0000) >> 16;
  pixel.b = (hex & 0x0000ff00) >> 8;
  pixel.a = hex & 0x000000ff;

  return pixel;
}

ByteImagePixel byte_image_pixel_opaque_from_rgb_hex(uint hex) {
  ByteImagePixel pixel;

  if (hex > 0xffffff) {
    throw("Hex (0x%lx) too large: maximum for RGB is 0xffffff", hex);
  }

  pixel.r = (hex & 0xff0000) >> 16;
  pixel.g = (hex & 0x00ff00) >> 8;
  pixel.b = hex & 0x0000ff;
  pixel.a = 0xff;

  return pixel;
}

ByteImagePixel byte_image_pixel_opaque_from_grayscale(byte gray) {
  ByteImagePixel pixel;

  pixel.r = pixel.g = pixel.b = gray;
  pixel.a = 0xff;

  return pixel;
}

ByteImagePixel byte_image_pixel_opaque_from_rgb_hex_string(char *hex) {
  long l;

  if (string_length(hex) != 6) {
    throw("String ('%s') must be in hex format 'rrggbb'", hex);
  }

  l = strtol(hex, null, 16);

  if (l < 0 || l > 0xffffff) {
    throw("Hex value must be between 0x000000 and 0xffffff: read 0x%lx", l);
  }

  return byte_image_pixel_opaque_from_rgb_hex((uint)l);
}

ByteImagePixel byte_image_pixel_opaque_black(void) {
  return byte_image_pixel_opaque_from_rgb_hex(0x000000);
}

ByteImagePixel byte_image_pixel_from_hsl(double h, double s, double l) {
  double r;
  double g;
  double b;
  double p;
  double q;
  ByteImagePixel pixel;

  if (s == 0) {
    r = g = b = l;
  } else {
    q = l < 0.5 ? l * (1 + s) : l + s - l * s;
    p = 2 * l - q;
    r = hue_to_rgb_(p, q, h + 1.0 / 3);
    g = hue_to_rgb_(p, q, h);
    b = hue_to_rgb_(p, q, h - 1.0 / 3);
  }

  pixel.r = (byte)round(r * 0xff);
  pixel.g = (byte)round(g * 0xff);
  pixel.b = (byte)round(b * 0xff);
  pixel.a = 0xff;

  return pixel;
}

bool byte_image_pixel_equals(ByteImagePixel x, ByteImagePixel y) {
  return x.r == y.r && x.g == y.g && x.b == y.b && x.a == y.a;
}

ByteImageYccaPixel byte_image_ycca_pixel_from_rgba(ByteImagePixel pixel) {
  double r;
  double g;
  double b;
  ByteImageYccaPixel ycca;

  r = pixel.r;
  g = pixel.g;
  b = pixel.b;

  ycca.y = byte_bound(0.299 * r + 0.587 * g + 0.114 * b);
  ycca.cb = byte_bound(128 - 0.168736 * r - 0.331264 * g + 0.5 * b);
  ycca.cr = byte_bound(128 + 0.5 * r - 0.418688 * g - 0.081312 * b);
  ycca.a = pixel.a;

  return ycca;
}

ByteImagePixel byte_image_pixel_from_ycca(ByteImageYccaPixel ycca) {
  double y;
  double cb;
  double cr;
  ByteImagePixel pixel;

  y = ycca.y;
  cb = ycca.cb - 128.0;
  cr = ycca.cr - 128.0;

  pixel.r = byte_bound(y + 1.402 * cr);
  pixel.g = byte_bound(y - 0.344136 * cb - 0.714136 * cr);
  pixel.b = byte_bound(y + 1.772 * cb);
  pixel.a = ycca.a;

  return pixel;
}

void byte_image_convert_ycca_to_rgba(ByteImage *this) {
  ulong i;
  ulong j;
  ByteImageYccaPixel ycca;
  ByteImagePixel rgba;

  for (i = 0; i < this->width; i++) {
    for (j = 0; j < this->height; j++) {
      ycca = byte_image_get_ycca(this, i, j);
      rgba = byte_image_pixel_from_ycca(ycca);
      byte_image_set(this, i, j, rgba);
    }
  }
}


static ulong new_(ByteImage **p_this, ulong width, ulong height) {
  ByteImage *this;

  if (width == 0) {
    throw("Width must be nonzero");
  }

  if (height== 0) {
    throw("Height must be nonzero");
  }

  this = newo(p_this);

  this->width = width;
  this->height = height;

  newm(&this->_pixels, height, width);

  return 0;
}

static void from_double_(Parallel *par) {
  bool fix;
  bool gray;
  bool last_row;
  bool last_t;
  bool ycc;
  byte c;
  byte cc;
  byte num_channels;
  ulong height;
  ulong i;
  ulong j;
  ulong t;
  ulong width;
  double add;
  double al;
  double error;
  double v;
  double **a;
  double **b;
  double **g;
  double **r;
  double **y;
  double **adds;
  ByteImagePixel rgba;
  ByteImageYccaPixel ycca;
  FromDoublePass_ *pass;
  DoubleImage *dither;
  ByteImage *this;

  pass = par->x;
  fix = pass->fix;
  width = pass->width;
  dither = pass->dither;
  num_channels = dither->numChannels;

  if (!fix) {
    if (pass->numThreads == 0) {
      lock(&pass->lock);

      if (pass->numThreads == 0) {
        new(&pass->_ends, par->numThreads - 1);
        newt3(&pass->_adds, par->numThreads - 1, num_channels, width);
        pass->numThreads = par->numThreads;
      }

      unlock(&pass->lock);
    }
  }

  last_t = (t = par->t) == par->numThreads - 1;

  if (!last_t) {
    if (!fix) {
      pass->_ends[t] = par->end;
    }

    adds = pass->_adds[t];
  }

  this = pass->this;
  ycc = pass->ycc;
  height = pass->height;
  gray = num_channels < 3;

  r = dither->r;
  g = dither->g;
  b = dither->b;
  a = dither->a;
  y = dither->y;

  al = 1;

  if (ycc) {
    ycca.a = 0xff;
  } else {
    rgba.a = 0xff;
  }

  for (j = par->start; j < par->end; j++) {
    for (i = 0; i < width; i++) {
      for (cc = 0; cc < num_channels; cc++) {
        c = num_channels - 1 - cc;

        if (c == 3 || (gray && c == 1)) {
          al = bound(a[i][j], 0, 1);

          if (ycc) {
            ycca.a = byte_bound(al * 0xff);
            debug_(ycca.a);
            error = al - (double)ycca.a / 0xff;
          } else {
            rgba.a = byte_bound(al * 0xff);
            debug_(rgba.a);
            error = al - (double)rgba.a / 0xff;
          }
        } else if (c == 2) {
          v = bound(al == 0 ? 0 : b[i][j] / al, 0, 1);

          if (ycc) {
            ycca.cr = byte_bound(v * 0xff);
            debug_(ycca.cr);
            error = v - (double)ycca.cr / 0xff;
          } else {
            rgba.b = srgb_linear_to_byte_gamma(v);
            debug_(rgba.b);
            error = v - srgb_byte_gamma_to_linear(rgba.b);
          }
        } else if (c == 1) {
          v = bound(al == 0 ? 0 : g[i][j] / al, 0, 1);

          if (ycc) {
            ycca.cb = byte_bound(v * 0xff);
            debug_(ycca.cb);
            error = v - (double)ycca.cb / 0xff;
          } else {
            rgba.g = srgb_linear_to_byte_gamma(v);
            debug_(rgba.g);
            error = v - srgb_byte_gamma_to_linear(rgba.g);
          }
        } else if (c == 0) {
          v = bound(al == 0 ? 0 : (gray ? y : r)[i][j] / al, 0, 1);

          if (ycc) {
            ycca.y = byte_bound(v * 0xff);
            debug_(ycca.y);
            error = v - (double)ycca.y / 0xff;
          } else {
            rgba.r = srgb_linear_to_byte_gamma(v);
            debug_(rgba.r);
            error = v - srgb_byte_gamma_to_linear(rgba.r);
          }
        } else {
          throw("impossible");
        }

        if (i < width - 1) {
          dither->_channels[c][i + 1][j] += 0.4375 * error;
        }

        if (j < height - 1) {
          last_row = j == par->end - 1;

          if (i > 0) {
            add = 0.1875 * error;

            if (!fix && last_row) {
              if (!last_t) {
                adds[c][i - 1] += add;
              }
            } else {
              dither->_channels[c][i - 1][j + 1] += add;
            }
          }

          add = 0.3125 * error;

          if (!fix && last_row) {
            if (!last_t) {
              adds[c][i] += add;
            }
          } else {
            dither->_channels[c][i][j + 1] += add;
          }

          if (i < width - 1) {
            add = 0.0625 * error;

            if (!fix && last_row) {
              if (!last_t) {
                adds[c][i + 1] += add;
              }
            } else {
              dither->_channels[c][i + 1][j] += add;
            }
          }
        }
      }

      if (num_channels < 3) {
        rgba.b = rgba.g = rgba.r;
      }

      if (ycc) {
        rgba = byte_image_pixel_from_ycca(ycca);
      }

      byte_image_set(this, i, j, rgba);
    }
  }
}

static void from_double_diff_(Parallel *par) {
  ulong height;
  ulong i;
  ulong j;
  double **r;
  double **g;
  double **b;
  FromDoubleDiffPass_ *pass;
  ByteImagePixel pixel;
  DoubleImage *diff;
  ByteImage *this;

  pass = par->x;

  this = pass->this;
  diff = pass->diff;
  height = this->height;
  r = diff->r;
  g = diff->g;
  b = diff->b;

  pixel.a = 0xff;

  for (i = par->start; i < par->end; i++) {
    for (j = 0; j < height; j++) {
      pixel.r = byte_encode_diff(r[i][j]);
      pixel.g = byte_encode_diff(g[i][j]);
      pixel.b = byte_encode_diff(b[i][j]);
      byte_image_set(this, i, j, pixel);
    }
  }
}

static void double_from_byte_diff_(Parallel *par) {
  ulong height;
  ulong i;
  ulong j;
  double **r;
  double **g;
  double **b;
  DoubleFromByteDiffPass_ *pass;
  ByteImagePixel pixel;
  ByteImage *other;
  DoubleImage *this;

  pass = par->x;

  this = pass->this;
  other = pass->other;
  height = this->height;
  r = this->r;
  g = this->g;
  b = this->b;

  for (i = par->start; i < par->end; i++) {
    for (j = 0; j < height; j++) {
      pixel = get_(other, i, j);
      r[i][j] = byte_decode_diff(pixel.r);
      g[i][j] = byte_decode_diff(pixel.g);
      b[i][j] = byte_decode_diff(pixel.b);
    }
  }
}

static ByteImagePixel get_(ByteImage *this, ulong i, ulong j) {
  return this->_pixels[j][i];
}

static byte get_r_(ByteImage *this, ulong i, ulong j) {
  return this->_pixels[j][i].r;
}

static byte get_g_(ByteImage *this, ulong i, ulong j) {
  return this->_pixels[j][i].g;
}

static byte get_b_(ByteImage *this, ulong i, ulong j) {
  return this->_pixels[j][i].b;
}

static byte get_a_(ByteImage *this, ulong i, ulong j) {
  return this->_pixels[j][i].a;
}

static byte get_y_(ByteImage *this, ulong i, ulong j) {
  ByteImagePixel pixel;

  pixel = get_(this, i, j);

  if (pixel.b != pixel.r || pixel.g != pixel.r) {
    throw("Pixel is not grayscale");
  }

  return pixel.r;
}

static void set_(ByteImage *this, ulong i, ulong j, ByteImagePixel pixel) {
  this->_pixels[j][i] = pixel;
}

static void set_r_(ByteImage *this, ulong i, ulong j, byte r) {
  this->_pixels[j][i].r = r;
}

static void set_g_(ByteImage *this, ulong i, ulong j, byte g) {
  this->_pixels[j][i].g = g;
}

static void set_b_(ByteImage *this, ulong i, ulong j, byte b) {
  this->_pixels[j][i].b = b;
}

static void set_a_(ByteImage *this, ulong i, ulong j, byte a) {
  check_(this, i, j);
  this->_pixels[j][i].a = a;
}

static void set_y_(ByteImage *this, ulong i, ulong j, byte y) {
  check_(this, i, j);
  this->_pixels[j][i].r = this->_pixels[j][i].g = this->_pixels[j][i].b = y;
}

static void check_(ByteImage *this, ulong i, ulong j) {
  if (i >= this->width) {
    throw("i (%lu) must be less than width (%lu)", i, this->width);
  }

  if (j >= this->height) {
    throw("j (%lu) must be less than height (%lu)", j, this->height);
  }
}

static double hue_to_rgb_(double p, double q, double t) {
  if (t < 0) {
    t += 1;
  }

  if (t > 1) {
    t -= 1;
  }

  if (t < 1.0 / 6) {
    return p + (q - p) * 6 * t;
  }

  if (t < 1.0 / 2) {
    return q;
  }

  if (t < 2.0 / 3) {
    return p + (q - p) * (2.0 / 3 - t) * 6;
  }

  return p;
}

static void clear_(Parallel *par) {
  ulong i;
  ulong j;
  ClearPass_ *pass;
  ByteImagePixel pixel;
  ByteImage *this;

  pass = par->x;
  this = pass->this;
  pixel = pass->pixel;

  for (j = par->start; j < par->end; j++) {
    for (i = 0; i < this->width; i++) {
      set_(this, i, j, pixel);
    }
  }
}

static void make_opaque_(Parallel *par) {
  ulong i;
  ulong j;
  ByteImage *this;

  this = par->x;

  for (j = par->start; j < par->end; j++) {
    for (i = 0; i < this->width; i++) {
      set_a_(this, i, j, 0xff);
    }
  }
}

static void double_from_byte_(Parallel *par) {
  bool ycc;
  byte c;
  byte cc;
  byte num_channels;
  ulong i;
  ulong j;
  ulong height;
  double al;
  double v;
  ByteImagePixel pixel;
  DoubleFromBytePass_ *pass;
  ByteImage *other;
  DoubleImage *this;

  pass = par->x;

  this = pass->this;
  other = pass->other;
  num_channels = this->numChannels;
  height = this->height;
  ycc = other->ycc;
  al = 1;

  for (i = par->start; i < par->end; i++) {
    for (j = 0; j < height; j++) {
      pixel = get_(other, i, j);

      for (cc = 0; cc < num_channels; cc++) {
        switch (c = num_channels - 1 - cc) {
          case 3:
            al = v = (double)pixel.a / 0xff;
            break;
          case 2:
            v = al * (ycc
                ? (double)pixel.b / 0xff
                : srgb_byte_gamma_to_linear(pixel.b));
            break;
          case 1:
            v = al * (ycc
                ? (double)pixel.g / 0xff
                : srgb_byte_gamma_to_linear(pixel.g));
            break;
          case 0:
            v = al * (ycc
                ? (double)pixel.r / 0xff
                : srgb_byte_gamma_to_linear(pixel.r));
            break;
          default:
            throw("impossible");
        }

        this->_channels[c][i][j] = v;
      }
    }
  }
}

static void transparent_(Parallel *par) {
  ulong i;
  ulong j;
  ulong height;
  OrPass_ *pass;
  ByteImage *this;

  pass = par->x;
  height = pass->height;
  this = pass->this;

  for (i = par->start; !pass->or && i < par->end; i++) {
    for (j = 0; !pass->or && j < height; j++) {
      if (get_(this, i, j).a != 0xff) {
        pass->or = true;

        return;
      }
    }
  }
}

static void grayscale_(Parallel *par) {
  ulong i;
  ulong j;
  ulong height;
  OrPass_ *pass;
  ByteImagePixel pixel;
  ByteImage *this;

  pass = par->x;
  height = pass->height;
  this = pass->this;

  for (i = par->start; !pass->or && i < par->end; i++) {
    for (j = 0; !pass->or && j < height; j++) {
      pixel = get_(this, i, j);

      if (pixel.b != pixel.r || pixel.g != pixel.r) {
        pass->or = true;

        return;
      }
    }
  }
}
