#include "DoubleImageTest.h"
#include <float.h>
#include <math.h>

START

TEST(DoubleImage, New) {
  byte c;
  ulong height;
  ulong i;
  ulong j;
  ulong width;
  DoubleImage *_di = null;

  width = 6;
  height = 7;

  double_image_new(&_di, width, height, 4);
  EXPECT_ULEQ(width, _di->width);
  EXPECT_ULEQ(height, _di->height);
  EXPECT_NONNULL(_di->_channels[0]);
  EXPECT_NONNULL(_di->_channels[1]);
  EXPECT_NONNULL(_di->_channels[2]);
  EXPECT_NONNULL(_di->_channels[3]);
  EXPECT_NONNULL(_di->r);
  EXPECT_NONNULL(_di->g);
  EXPECT_NONNULL(_di->b);
  EXPECT_NONNULL(_di->a);
  EXPECT_NULL(_di->y);

  for (j = 0; j < height; j++) {
    for (i = 0; i < width; i++) {
      for (c = 0; c < 4; c++) {
        EXPECT_DEQ(c == 3 ? 1 : 0, _di->_channels[c][i][j]);
      }

      EXPECT_DEQ(0, _di->r[i][j]);
      EXPECT_DEQ(0, _di->g[i][j]);
      EXPECT_DEQ(0, _di->b[i][j]);
      EXPECT_DEQ(1, _di->a[i][j]);
    }
  }

  double_image_delete(&_di);

  double_image_new(&_di, width, height, 3);
  EXPECT_ULEQ(width, _di->width);
  EXPECT_ULEQ(height, _di->height);
  EXPECT_NONNULL(_di->_channels[0]);
  EXPECT_NONNULL(_di->_channels[1]);
  EXPECT_NONNULL(_di->_channels[2]);
  EXPECT_NONNULL(_di->r);
  EXPECT_NONNULL(_di->g);
  EXPECT_NONNULL(_di->b);
  EXPECT_NULL(_di->a);
  EXPECT_NULL(_di->y);

  for (j = 0; j < height; j++) {
    for (i = 0; i < width; i++) {
      for (c = 0; c < 3; c++) {
        EXPECT_DEQ(0, _di->_channels[c][i][j]);
      }

      EXPECT_DEQ(0, _di->r[i][j]);
      EXPECT_DEQ(0, _di->g[i][j]);
      EXPECT_DEQ(0, _di->b[i][j]);
    }
  }

  double_image_delete(&_di);

  double_image_new(&_di, width, height, 2);
  EXPECT_ULEQ(width, _di->width);
  EXPECT_ULEQ(height, _di->height);
  EXPECT_NONNULL(_di->_channels[0]);
  EXPECT_NONNULL(_di->_channels[1]);
  EXPECT_NULL(_di->r);
  EXPECT_NULL(_di->g);
  EXPECT_NULL(_di->b);
  EXPECT_NONNULL(_di->a);
  EXPECT_NONNULL(_di->y);

  for (j = 0; j < height; j++) {
    for (i = 0; i < width; i++) {
      for (c = 0; c < 2; c++) {
        EXPECT_DEQ(c == 1 ? 1 : 0, _di->_channels[c][i][j]);
      }

      EXPECT_DEQ(0, _di->y[i][j]);
      EXPECT_DEQ(1, _di->a[i][j]);
    }
  }

  double_image_delete(&_di);

  double_image_new(&_di, width, height, 1);
  EXPECT_ULEQ(width, _di->width);
  EXPECT_ULEQ(height, _di->height);
  EXPECT_NONNULL(_di->_channels[0]);
  EXPECT_NONNULL(_di->y);
  EXPECT_NULL(_di->r);
  EXPECT_NULL(_di->g);
  EXPECT_NULL(_di->b);
  EXPECT_NULL(_di->a);

  for (j = 0; j < height; j++) {
    for (i = 0; i < width; i++) {
      EXPECT_DEQ(0, _di->_channels[0][i][j]);
      EXPECT_DEQ(0, _di->y[i][j]);
    }
  }

  double_image_delete(&_di);
}

TEST(DoubleImage, NewRgba) {
  byte c;
  ulong height;
  ulong i;
  ulong j;
  ulong width;
  DoubleImage *_di = null;

  width = 6;
  height = 7;

  double_image_new_rgba(&_di, width, height);
  EXPECT_ULEQ(width, _di->width);
  EXPECT_ULEQ(height, _di->height);
  EXPECT_NONNULL(_di->_channels[0]);
  EXPECT_NONNULL(_di->_channels[1]);
  EXPECT_NONNULL(_di->_channels[2]);
  EXPECT_NONNULL(_di->_channels[3]);
  EXPECT_NONNULL(_di->r);
  EXPECT_NONNULL(_di->g);
  EXPECT_NONNULL(_di->b);
  EXPECT_NONNULL(_di->a);
  EXPECT_NULL(_di->y);

  for (j = 0; j < height; j++) {
    for (i = 0; i < width; i++) {
      for (c = 0; c < 4; c++) {
        EXPECT_DEQ(c == 3 ? 1 : 0, _di->_channels[c][i][j]);
      }

      EXPECT_DEQ(0, _di->r[i][j]);
      EXPECT_DEQ(0, _di->g[i][j]);
      EXPECT_DEQ(0, _di->b[i][j]);
      EXPECT_DEQ(1, _di->a[i][j]);
    }
  }

  double_image_delete(&_di);
}

TEST(DoubleImage, NewRgb) {
  byte c;
  ulong height;
  ulong i;
  ulong j;
  ulong width;
  DoubleImage *_di = null;

  width = 6;
  height = 7;

  double_image_new_rgb(&_di, width, height);
  EXPECT_ULEQ(width, _di->width);
  EXPECT_ULEQ(height, _di->height);
  EXPECT_NONNULL(_di->_channels[0]);
  EXPECT_NONNULL(_di->_channels[1]);
  EXPECT_NONNULL(_di->_channels[2]);
  EXPECT_NONNULL(_di->r);
  EXPECT_NONNULL(_di->g);
  EXPECT_NONNULL(_di->b);
  EXPECT_NULL(_di->a);
  EXPECT_NULL(_di->y);

  for (j = 0; j < height; j++) {
    for (i = 0; i < width; i++) {
      for (c = 0; c < 3; c++) {
        EXPECT_DEQ(0, _di->_channels[c][i][j]);
      }

      EXPECT_DEQ(0, _di->r[i][j]);
      EXPECT_DEQ(0, _di->g[i][j]);
      EXPECT_DEQ(0, _di->b[i][j]);
    }
  }

  double_image_delete(&_di);
}

TEST(DoubleImage, NewYA) {
  ulong height;
  ulong i;
  ulong j;
  ulong width;
  DoubleImage *_di = null;

  width = 6;
  height = 7;

  double_image_new_ya(&_di, width, height);
  EXPECT_ULEQ(width, _di->width);
  EXPECT_ULEQ(height, _di->height);
  EXPECT_NONNULL(_di->_channels[0]);
  EXPECT_NONNULL(_di->_channels[1]);
  EXPECT_NONNULL(_di->y);
  EXPECT_NONNULL(_di->a);
  EXPECT_NULL(_di->r);
  EXPECT_NULL(_di->g);
  EXPECT_NULL(_di->b);

  for (j = 0; j < height; j++) {
    for (i = 0; i < width; i++) {
      EXPECT_DEQ(0, _di->_channels[0][i][j]);
      EXPECT_DEQ(0, _di->y[i][j]);
      EXPECT_DEQ(1, _di->_channels[1][i][j]);
      EXPECT_DEQ(1, _di->a[i][j]);
    }
  }

  double_image_delete(&_di);
}

TEST(DoubleImage, NewY) {
  ulong height;
  ulong i;
  ulong j;
  ulong width;
  DoubleImage *_di = null;

  width = 6;
  height = 7;

  double_image_new_y(&_di, width, height);
  EXPECT_ULEQ(width, _di->width);
  EXPECT_ULEQ(height, _di->height);
  EXPECT_NONNULL(_di->_channels[0]);
  EXPECT_NONNULL(_di->y);
  EXPECT_NULL(_di->r);
  EXPECT_NULL(_di->g);
  EXPECT_NULL(_di->b);
  EXPECT_NULL(_di->a);

  for (j = 0; j < height; j++) {
    for (i = 0; i < width; i++) {
      EXPECT_DEQ(0, _di->_channels[0][i][j]);
      EXPECT_DEQ(0, _di->y[i][j]);
    }
  }

  double_image_delete(&_di);
}

TEST(DoubleImages, NewChannels) {
  byte c;
  byte num_channels;
  ulong width;
  ulong height;
  DoubleImage *_di = null;
  DoubleImage **_channels = null;

  width = 6;
  height = 7;

  double_image_new_rgba(&_di, width, height);
  _di->r[3][4] = 0.5;
  _di->g[4][5] = 0.1;
  _di->b[2][3] = 0.4;
  _di->a[1][2] = 0.9;
  num_channels = double_images_new_split_channels(&_channels, _di);
  double_image_delete(&_di);

  EXPECT_ULEQ(4, num_channels);

  for (c = 0; c < num_channels; c++) {
    EXPECT_ULEQ(1, _channels[c]->numChannels);
    EXPECT_ULEQ(width, _channels[c]->width);
    EXPECT_ULEQ(height, _channels[c]->height);
  }

  EXPECT_DEQ(0.5, _channels[0]->y[3][4]);
  EXPECT_DEQ(0.1, _channels[1]->y[4][5]);
  EXPECT_DEQ(0.4, _channels[2]->y[2][3]);
  EXPECT_DEQ(0.9, _channels[3]->y[1][2]);

  delete(&_channels, num_channels, double_image_delete);

  double_image_new_rgb(&_di, width, height);
  num_channels = double_images_new_split_channels(&_channels, _di);
  double_image_delete(&_di);

  EXPECT_ULEQ(3, num_channels);

  for (c = 0; c < num_channels; c++) {
    EXPECT_ULEQ(1, _channels[c]->numChannels);
    EXPECT_ULEQ(width, _channels[c]->width);
    EXPECT_ULEQ(height, _channels[c]->height);
  }

  delete(&_channels, num_channels, double_image_delete);
}

TEST(DoubleImages, NewSplitAlpha) {
  ulong width;
  ulong height;
  DoubleImage *_di = null;
  DoubleImage *_alpha = null;
  DoubleImage *_non_alpha = null;

  width = 6;
  height = 7;

  double_image_new_rgba(&_di, width, height);
  _di->r[3][4] = 0.5;
  _di->g[4][5] = 0.1;
  _di->b[2][3] = 0.4;
  _di->a[1][2] = 0.9;

  double_images_new_split_alpha(&_alpha, &_non_alpha, _di);
  double_image_delete(&_di);
  EXPECT_DEQ(0.5, _non_alpha->r[3][4]);
  EXPECT_DEQ(0.1, _non_alpha->g[4][5]);
  EXPECT_DEQ(0.4, _non_alpha->b[2][3]);
  EXPECT_DEQ(0.9, _alpha->y[1][2]);

  double_image_new_add_alpha(&_di, _alpha, _non_alpha);
  double_image_delete(&_non_alpha);
  double_image_delete(&_alpha);
  _di->r[3][4] = 0.5;
  _di->g[4][5] = 0.1;
  _di->b[2][3] = 0.4;
  _di->a[1][2] = 0.9;
  double_image_delete(&_di);
}

TEST(DoubleImage, NewCopy) {
  ulong width;
  ulong height;
  DoubleImage *_di = null;
  DoubleImage *_copy = null;

  width = 6;
  height = 7;

  double_image_new_rgba(&_di, width, height);
  _di->r[3][4] = 0.5;
  _di->g[4][5] = 0.1;
  _di->b[2][3] = 0.4;
  _di->a[1][2] = 0.9;
  double_image_new_copy(&_copy, _di);
  double_image_delete(&_di);
  EXPECT_ULEQ(4, _copy->numChannels);
  EXPECT_ULEQ(width, _copy->width);
  EXPECT_ULEQ(height, _copy->height);
  EXPECT_DEQ(0.5, _copy->r[3][4]);
  EXPECT_DEQ(0.1, _copy->g[4][5]);
  EXPECT_DEQ(0.4, _copy->b[2][3]);
  EXPECT_DEQ(0.9, _copy->a[1][2]);
  double_image_delete(&_copy);

  double_image_new_rgb(&_di, width, height);
  _di->r[3][4] = 0.5;
  _di->g[4][5] = 0.1;
  _di->b[2][3] = 0.4;
  double_image_new_copy(&_copy, _di);
  double_image_delete(&_di);
  EXPECT_ULEQ(3, _copy->numChannels);
  EXPECT_ULEQ(width, _copy->width);
  EXPECT_ULEQ(height, _copy->height);
  EXPECT_DEQ(0.5, _copy->r[3][4]);
  EXPECT_DEQ(0.1, _copy->g[4][5]);
  EXPECT_DEQ(0.4, _copy->b[2][3]);
  double_image_delete(&_copy);

  double_image_new_y(&_di, width, height);
  _di->y[3][4] = 0.5;
  double_image_new_copy(&_copy, _di);
  double_image_delete(&_di);
  EXPECT_ULEQ(1, _copy->numChannels);
  EXPECT_ULEQ(width, _copy->width);
  EXPECT_ULEQ(height, _copy->height);
  EXPECT_DEQ(0.5, _copy->y[3][4]);
  double_image_delete(&_copy);
}

TEST(DoubleImage, NewCopy) {
  ulong width;
  ulong height;
  DoubleImage *_di = null;
  DoubleImage *_copy = null;

  width = 6;
  height = 7;

  double_image_new_rgba(&_di, width, height);
  _di->r[3][4] = 0.5;
  _di->g[4][5] = 0.1;
  _di->b[2][3] = 0.4;
  _di->a[1][2] = 0.9;
  double_image_new_transpose(&_copy, _di);
  double_image_delete(&_di);
  EXPECT_ULEQ(4, _copy->numChannels);
  EXPECT_ULEQ(height, _copy->width);
  EXPECT_ULEQ(width, _copy->height);
  EXPECT_DEQ(0.5, _copy->r[4][3]);
  EXPECT_DEQ(0.1, _copy->g[5][4]);
  EXPECT_DEQ(0.4, _copy->b[3][2]);
  EXPECT_DEQ(0.9, _copy->a[2][1]);
  double_image_delete(&_copy);

  double_image_new_rgb(&_di, width, height);
  _di->r[3][4] = 0.5;
  _di->g[4][5] = 0.1;
  _di->b[2][3] = 0.4;
  double_image_new_transpose(&_copy, _di);
  double_image_delete(&_di);
  EXPECT_ULEQ(3, _copy->numChannels);
  EXPECT_ULEQ(height, _copy->width);
  EXPECT_ULEQ(width, _copy->height);
  EXPECT_DEQ(0.5, _copy->r[4][3]);
  EXPECT_DEQ(0.1, _copy->g[5][4]);
  EXPECT_DEQ(0.4, _copy->b[3][2]);
  double_image_delete(&_copy);

  double_image_new_y(&_di, width, height);
  _di->y[3][4] = 0.5;
  double_image_new_transpose(&_copy, _di);
  double_image_delete(&_di);
  EXPECT_ULEQ(1, _copy->numChannels);
  EXPECT_ULEQ(height, _copy->width);
  EXPECT_ULEQ(width, _copy->height);
  EXPECT_DEQ(0.5, _copy->y[4][3]);
  double_image_delete(&_copy);
}

TEST(DoubleImage, NewCrop) {
  byte c;
  ulong i;
  ulong j;
  DoubleImage *_di = null;
  DoubleImage *_di2 = null;

  double_image_new_rgba(&_di, 4, 5);

  for (c = 0; c < 4; c++) {
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 5; j++) {
        _di->_channels[c][i][j] = (i + 1.0) / 7 + (j + 1.0) / 11 + c / 13.0;
      }
    }
  }

  double_image_new_crop(&_di2, _di, 2, 2, 1, 3);
  double_image_delete(&_di);

  for (c = 0; c < 4; c++) {
    for (i = 0; i < 2; i++) {
      for (j = 0; j < 3; j++) {
        EXPECT_DEQ(
            (i + 3.0) / 7 + (j + 2.0) / 11 + c / 13.0,
            _di2->_channels[c][i][j]);
      }
    }
  }

  double_image_delete(&_di2);
}

TEST(DoubleImage, NewUncrop) {
  byte c;
  ulong i;
  ulong j;
  double background[4];
  DoubleImage *_di = null;
  DoubleImage *_di2 = null;

  double_image_new_rgba(&_di, 4, 5);

  for (c = 0; c < 4; c++) {
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 5; j++) {
        _di->_channels[c][i][j] = (i + 1.0) / 7 + (j + 1.0) / 11 + c / 13.0;
      }
    }
  }

  background[0] = 0.1;
  background[1] = 0.9;
  background[2] = 0.7;
  background[3] = 0.4;

  double_image_new_uncrop(&_di2, _di, -1, 3, -2, 10, background);
  double_image_delete(&_di);

  for (c = 0; c < 4; c++) {
    for (i = 0; i < 3; i++) {
      for (j = 0; j < 10; j++) {
        EXPECT_DEQ(
            i < 1 || j < 2 || j >= 7
            ? background[c]
            : (double)i / 7 + (j - 1.0) / 11 + c / 13.0,
            _di2->_channels[c][i][j]);
      }
    }
  }

  double_image_delete(&_di2);
}

TEST(DoubleImage, NewAddSubtractAddQuadrature) {
  byte c;
  ulong i;
  ulong j;
  double x;
  double y;
  DoubleImage *_a = null;
  DoubleImage *_b = null;
  DoubleImage *_sum = null;
  DoubleImage *_diff = null;
  DoubleImage *_quad = null;

  double_image_new_rgba(&_a, 3, 4);
  double_image_new_rgba(&_b, 3, 4);

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 4; j++) {
      for (c = 0; c < 4; c++) {
        _a->_channels[c][i][j] = (i + 1.0) / (j + 1.5) + (c + 1.7) / 11.3;
        _b->_channels[c][i][j] = (i - 3.4) / (j + 1.6) + (c + 1.9) / 3.14;
      }
    }
  }

  double_image_new_add(&_sum, _a, _b);
  double_image_new_subtract(&_diff, _a, _b);
  double_image_new_add_quadrature(&_quad, _a, _b);

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 4; j++) {
      for (c = 0; c < 4; c++) {
        x = (i + 1.0) / (j + 1.5) + (c + 1.7) / 11.3;
        y = (i - 3.4) / (j + 1.6) + (c + 1.9) / 3.14;
        EXPECT_DEQ(x + y, _sum->_channels[c][i][j]);
        EXPECT_DEQ(x - y, _diff->_channels[c][i][j]);
        EXPECT_DEQ(sqrt(x * x + y * y), _quad->_channels[c][i][j]);
      }
    }
  }

  double_image_delete(&_quad);
  double_image_delete(&_diff);
  double_image_delete(&_sum);
  double_image_delete(&_b);
  double_image_delete(&_a);
}

TEST(DoubleImage, Print) {
  char *_e = null;
  char *_s = null;
  DoubleImage *_ = null;


  double_image_new(&_, 1, 2, 4);
  _->_channels[0][0][1] = 0.6;
  _->_channels[1][0][0] = 0.3;
  _->_channels[2][0][1] = 0.5;
  _->_channels[3][0][0] = 0.1;
  aprint(&_s, 0, double_image, _);
  string_new_f(
      &_e,
      "%s%s",
      "[DoubleImage]\n. Width: 1\n. Height: 2\n. Number of channels: 4\n"
      ". [Contents]\n. . [Channel 0]\n. . . [Row 0]\n. . . . [0] 0\n"
      ". . . [/Row 0]\n. . . [Row 1]\n. . . . [0] 0.6\n. . . [/Row 1]\n"
      ". . [/Channel 0]\n. . [Channel 1]\n. . . [Row 0]\n. . . . [0] 0.3\n"
      ". . . [/Row 0]\n. . . [Row 1]\n. . . . [0] 0\n. . . [/Row 1]\n"
      ". . [/Channel 1]\n",
      ". . [Channel 2]\n. . . [Row 0]\n. . . . [0] 0\n. . . [/Row 0]\n"
      ". . . [Row 1]\n. . . . [0] 0.5\n. . . [/Row 1]\n. . [/Channel 2]\n"
      ". . [Channel 3]\n. . . [Row 0]\n. . . . [0] 0.1\n. . . [/Row 0]\n"
      ". . . [Row 1]\n. . . . [0] 1\n. . . [/Row 1]\n. . [/Channel 3]\n"
      ". [/Contents]\n[/DoubleImage]\n",
      _s);
  EXPECT_STREQ(_e, _s);
  string_delete(&_s);
  string_delete(&_e);

  double_image_delete(&_);
}

TEST(DoubleImage, HasAlphaIsGrayscale) {
  DoubleImage *_ = null;

  double_image_new_rgba(&_, 3, 4);
  EXPECT_TRUE(double_image_has_alpha(_));
  EXPECT_FALSE(double_image_is_grayscale(_));
  double_image_delete(&_);

  double_image_new_rgb(&_, 3, 4);
  EXPECT_FALSE(double_image_has_alpha(_));
  EXPECT_FALSE(double_image_is_grayscale(_));
  double_image_delete(&_);

  double_image_new_y(&_, 3, 4);
  EXPECT_FALSE(double_image_has_alpha(_));
  EXPECT_TRUE(double_image_is_grayscale(_));
  double_image_delete(&_);
}

TEST(DoubleImage, ContainsTransparency) {
  DoubleImage *_di = null;

  double_image_new_rgb(&_di, 2, 3);
  EXPECT_FALSE(double_image_contains_transparency(_di));
  double_image_delete(&_di);

  double_image_new_rgba(&_di, 2, 3);
  EXPECT_FALSE(double_image_contains_transparency(_di));
  _di->a[1][2] = 0.3;
  EXPECT_TRUE(double_image_contains_transparency(_di));
  double_image_delete(&_di);
}

TEST(DoubleImage, MaxRgbMaxAbsRgbMeanAbsRgb) {
  byte c;
  ulong i;
  ulong j;
  ulong u;
  double d;
  double max;
  double max_abs;
  double sum_abs;
  Random *_random = null;
  DoubleImage *_di = null;

  random_new(&_random);
  double_image_new_rgba(&_di, 8, 9);

  for (u = 0; u < 100; u++) {
    for (max = -DBL_MAX, max_abs = -DBL_MAX, sum_abs = 0, c = 0; c < 4; c++) {
      for (i = 0; i < 8; i++) {
        for (j = 0; j < 9; j++) {
          d = random_double_unit_cc(_random) - 0.5;
          _di->_channels[c][i][j] = d;

          if (c < 3) {
            max = maximum(max, d);
            max_abs = maximum(max_abs, fabs(d));
            sum_abs += fabs(d);
          }
        }
      }
    }

    EXPECT_DEQ(max, double_image_max_rgb(_di));
    EXPECT_DEQ(max_abs, double_image_max_abs_rgb(_di));
    EXPECT_DEQ(sum_abs / 216, double_image_mean_abs_rgb(_di));
  }

  double_image_delete(&_di);
  random_delete(&_random);
}

TEST(DoubleImage, MakeOpaque) {
  ulong i;
  ulong j;
  DoubleImage *_di = null;

  double_image_new_rgba(&_di, 2, 3);

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      _di->a[i][j] = i + j;
    }
  }

  double_image_make_opaque(_di);

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      EXPECT_DEQ(1, _di->a[i][j]);
    }
  }

  double_image_delete(&_di);
}

TEST(DoubleImage, Bound) {
  DoubleImage *_di = null;

  double_image_new_rgba(&_di, 1, 1);
  _di->r[0][0] = -3.14;
  _di->g[0][0] = -1.2;
  _di->b[0][0] = 1.2;
  _di->a[0][0] = 3.14;
  double_image_bound(_di, -2.1, 1.1);
  EXPECT_DEQ(-2.1, _di->r[0][0]);
  EXPECT_DEQ(-1.2, _di->g[0][0]);
  EXPECT_DEQ(1.1, _di->b[0][0]);
  EXPECT_DEQ(1.1, _di->a[0][0]);
  double_image_delete(&_di);
}

TEST(DoubleImage, AutoScale) {
  DoubleImage *_di = null;

  double_image_new_rgb(&_di, 1, 1);
  _di->r[0][0] = -2;
  _di->g[0][0] = -1;
  _di->b[0][0] = 2;
  double_image_auto_scale(_di);
  EXPECT_NEAR(0, _di->r[0][0], 1e-15);
  EXPECT_DEQ(0.25, _di->g[0][0]);
  EXPECT_DEQ(1, _di->b[0][0]);
  double_image_delete(&_di);
}

TEST(DoubleImage, Scale) {
  DoubleImage *_di = null;

  double_image_new_rgba(&_di, 1, 1);
  _di->r[0][0] = -3.14;
  _di->g[0][0] = -1.2;
  _di->b[0][0] = 1.2;
  _di->a[0][0] = 3.14;
  double_image_scale_rgb(_di, 2.5);
  EXPECT_NEAR(-7.85, _di->r[0][0], 1e-15);
  EXPECT_DEQ(-3, _di->g[0][0]);
  EXPECT_DEQ(3, _di->b[0][0]);
  EXPECT_DEQ(3.14, _di->a[0][0]);
  double_image_delete(&_di);
}


TEST(DoubleImage, LinearGamma) {
  DoubleImage *_di = null;

  double_image_new_rgb(&_di, 1, 1);
  _di->r[0][0] = 0.25;
  _di->g[0][0] = 0.5;
  _di->b[0][0] = 0.75;
  double_image_linear_to_gamma(_di);
  EXPECT_NEAR(0.537098730483194, _di->r[0][0], 1e-15);
  EXPECT_NEAR(0.735356983052449, _di->g[0][0], 1e-15);
  EXPECT_NEAR(0.8808250210903, _di->b[0][0], 1e-15);
  double_image_gamma_to_linear(_di);
  EXPECT_NEAR(0.25, _di->r[0][0], 1e-15);
  EXPECT_NEAR(0.5, _di->g[0][0], 1e-15);
  EXPECT_NEAR(0.75, _di->b[0][0], 1e-15);
  double_image_delete(&_di);
}

FINISH
