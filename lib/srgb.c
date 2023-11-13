#include "srgb.h"
#include <math.h>

#define B_ 3295

static bool initialized_ = false;
static Lock lock_ = LOCK_INITIALIZER;
static byte bs_[B_ + 1];
static double ds_[256];

static void ensure_initialized_(void);


byte srgb_linear_to_byte_gamma(double v) {
  ensure_initialized_();

  return bs_[(ushort)round(bound(v, 0, 1) * B_)];
}

double srgb_byte_gamma_to_linear(byte b) {
  ensure_initialized_();

  return ds_[b];
}

double srgb_double_linear_to_gamma(double v) {
  v = bound(v, 0, 1);

  if (v <= 0.0031308) {
    return v * 12.92;
  }

  return 1.055 * pow(v, 1.0 / 2.4) - 0.055;
}

double srgb_double_gamma_to_linear(double v) {
  v = bound(v, 0, 1);

  if (v <= 0.04045) {
    return v / 12.92;
  }

  return pow((v + 0.055) / 1.055, 2.4);
}


static void ensure_initialized_(void) {
  ushort u;

  if (!initialized_) {

    lock(&lock_);

    if (!initialized_) {
      for (u = 0; u < 256; u++) {
        ds_[u] = srgb_double_gamma_to_linear(u / 255.0);
      }

      for (u = 0; u <= B_; u++) {
        bs_[u] = (byte)round(srgb_double_linear_to_gamma((double)u / B_) * 255);
      }

      initialized_ = true;
    }

    unlock(&lock_);
  }
}
