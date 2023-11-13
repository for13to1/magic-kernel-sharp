#include "byte.h"

static bool initialized_ = false;
static Lock lock_ = LOCK_INITIALIZER;
static byte bs_[511];
static short ds_[256];

static void ensure_diff_(void);


byte byte_bound(double v) {
  return v < 0.5 ? 0 : v >= 254.5 ? 255 : round(v);
}

byte byte_encode_diff(double diff) {
  ensure_diff_();

  if (diff < -1 || diff > +1) {
    throw("Diff (%.16g) not in range [-1, +1]", diff);
  }

  return bs_[(short)round((diff + 1) * 255)];
}

double byte_decode_diff(byte by) {
  ensure_diff_();

  return ds_[by] / 255.0;
}


static void ensure_diff_(void) {
  byte *_w = null;
  ushort b;
  short a;
  short d;
  short m;
  short p;
  short r;
  short s;
  short t;

  if (!initialized_) {
    lock(&lock_);

    if (!initialized_) {
      new(&_w, 511);

      for (b = 1; b < 256; b++) {
        r = (short)b - 128;
        a = abs(r);
        d = a + a * a * a / 16003;

        if (d > 254) {
          throw("impossible %d", d);
        }

        if (r < 0) {
          d = -d;
        }

        ds_[b] = d;
        s = d + 255;
        _w[s] = (byte)b;
      }

      ds_[0] = ds_[1];

      for (d = -255; d <= +255; d++) {
        s = d + 255;

        if (_w[s] != 0) {
          bs_[s] = _w[s];
        } else {
          for (p = -1, m = -1, t = s + 1, r = s - 1; p < 0 && m < 0; t++, r--) {
            if (t < 511 && _w[t] > 0) {
              p = _w[t];
            }

            if (r >= 0 && _w[r] > 0) {
              m = _w[r];
            }
          }

          bs_[s] = (byte)(p < 0 ? m : m < 0 || d > 0 ? p : m);
        }
      }

      deletev(&_w);

      initialized_ = true;
    }

    unlock(&lock_);
  }
}
