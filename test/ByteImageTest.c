#include "ByteImageTest.h"

START

TEST(ByteImage, New) {
  ulong i;
  ulong j;
  ByteImagePixel pixel;
  ByteImage *_bi = null;

  byte_image_new(&_bi, 3, 4);
  EXPECT_ULEQ(3, _bi->width);
  EXPECT_ULEQ(4, _bi->height);

  pixel = byte_image_pixel_opaque_black();

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 4; j++) {
      EXPECT_TRUE(byte_image_pixel_equals(pixel, byte_image_get(_bi, i, j)));
    }
  }

  byte_image_delete(&_bi);
}

TEST(ByteImage, NewTransparent) {
  ulong i;
  ulong j;
  ByteImagePixel pixel;
  ByteImage *_bi = null;

  byte_image_new_transparent(&_bi, 3, 4);
  EXPECT_ULEQ(3, _bi->width);
  EXPECT_ULEQ(4, _bi->height);

  pixel = byte_image_pixel_from_rgba_hex(0x00000000);

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 4; j++) {
      EXPECT_TRUE(byte_image_pixel_equals(pixel, byte_image_get(_bi, i, j)));
    }
  }

  byte_image_delete(&_bi);
}

TEST(ByteImage, NewFromDouble) {
  ByteImagePixel pixel;
  DoubleImage *_di = null;
  ByteImage *_bi = null;

  double_image_new_rgb(&_di, 2, 3);
  byte_image_new_from_double(&_bi, _di);
  test_save_load(ByteImage, byte_image, _bi);
  double_image_delete(&_di);
  EXPECT_ULEQ(2, _bi->width);
  EXPECT_ULEQ(3, _bi->height);
  EXPECT_ULEQ(0, byte_image_get_r(_bi, 0, 2));
  EXPECT_ULEQ(0, byte_image_get_g(_bi, 1, 2));
  EXPECT_ULEQ(0, byte_image_get_b(_bi, 0, 1));
  EXPECT_ULEQ(255, byte_image_get_a(_bi, 1, 1));
  double_image_new_from_byte(&_di, _bi);
  byte_image_delete(&_bi);
  EXPECT_ULEQ(2, _di->width);
  EXPECT_ULEQ(3, _di->height);
  EXPECT_NULL(_di->a);
  EXPECT_NULL(_di->g);
  EXPECT_NULL(_di->b);
  EXPECT_TRUE(double_image_is_grayscale(_di));
  EXPECT_DEQ(0, _di->y[0][2]);
  EXPECT_DEQ(0, _di->y[1][2]);
  EXPECT_DEQ(0, _di->y[0][1]);
  double_image_delete(&_di);

  double_image_new_rgba(&_di, 2, 3);
  byte_image_new_from_double(&_bi, _di);
  test_save_load(ByteImage, byte_image, _bi);
  double_image_delete(&_di);
  EXPECT_ULEQ(2, _bi->width);
  EXPECT_ULEQ(3, _bi->height);
  EXPECT_ULEQ(0, byte_image_get_r(_bi, 0, 2));
  EXPECT_ULEQ(0, byte_image_get_g(_bi, 1, 2));
  EXPECT_ULEQ(0, byte_image_get_b(_bi, 0, 1));
  EXPECT_ULEQ(255, byte_image_get_a(_bi, 1, 1));
  double_image_new_from_byte(&_di, _bi);
  byte_image_delete(&_bi);
  EXPECT_ULEQ(2, _di->width);
  EXPECT_ULEQ(3, _di->height);
  EXPECT_NULL(_di->a);
  EXPECT_DEQ(0, _di->y[0][2]);
  EXPECT_DEQ(0, _di->y[1][2]);
  EXPECT_DEQ(0, _di->y[0][1]);
  double_image_delete(&_di);

  /* Test premultiplied alpha.
   */
  double_image_new_rgba(&_di, 1, 2);
  _di->r[0][0] = 0.5;
  _di->g[0][0] = 0.25;
  _di->b[0][0] = 0.75;
  _di->a[0][0] = 0.8;
  _di->r[0][1] = 0.5;
  _di->g[0][1] = 0.25;
  _di->b[0][1] = 0.75;
  _di->a[0][1] = 0;
  byte_image_new_from_double(&_bi, _di);
  test_save_load(ByteImage, byte_image, _bi);
  double_image_delete(&_di);
  pixel = byte_image_get(_bi, 0, 0);
  EXPECT_ULEQ(207, pixel.r);
  EXPECT_ULEQ(152, pixel.g);
  EXPECT_ULEQ(248, pixel.b);
  EXPECT_ULEQ(204, pixel.a);
  pixel = byte_image_get(_bi, 0, 1);
  EXPECT_ULEQ(0, pixel.r);
  EXPECT_ULEQ(0, pixel.g);
  EXPECT_ULEQ(0, pixel.b);
  EXPECT_ULEQ(0, pixel.a);
  double_image_new_from_byte(&_di, _bi);
  byte_image_delete(&_bi);
  EXPECT_NEAR(0.5, _di->r[0][0], 1.2e-3);
  EXPECT_NEAR(0.25, _di->g[0][0], 1.2e-3);
  EXPECT_NEAR(0.75, _di->b[0][0], 1e-3);
  EXPECT_NEAR(204.0 / 255, _di->a[0][0], 1e-15);
  EXPECT_DEQ(0, _di->r[0][1]);
  EXPECT_DEQ(0, _di->g[0][1]);
  EXPECT_DEQ(0, _di->b[0][1]);
  EXPECT_DEQ(0, _di->a[0][1]);
  double_image_delete(&_di);
}

TEST(ByteImage, NewFromDoubleDiff) {
  byte a;
  byte c;
  ulong i;
  ulong j;
  short ss[] = {-254, -239, -220, -168, -101, -99, -68, -40, -27, -25, -9, -1,
    0, 1, 3, 25, 27, 38, 40, 109, 111, 189, 193, 254};
  ByteImage *_bi = null;
  DoubleImage *_di_diff = null;
  DoubleImage *_di_final = null;
  DoubleImage *_di_orig = null;

  double_image_new_rgb(&_di_orig, 2, 4);
  _di_orig->gamma = true;

  for (a = 0, c = 0; c < 3; c++) {
    for (i = 0; i < 2; i++) {
      for (j = 0; j < 3; j++, a++) {
        _di_orig->_channels[c][i][j] = ss[a] / 255.0;
      }
    }
  }

  byte_image_new_from_double_diff(&_bi, _di_orig);
  test_save_load(ByteImage, byte_image, _bi);
  double_image_new_from_byte_diff(&_di_final, _bi);
  byte_image_delete(&_bi);
  double_image_new_subtract(&_di_diff, _di_final, _di_orig);
  EXPECT_DEQ(0, double_image_max_abs_rgb(_di_diff));
  double_image_delete(&_di_diff);
  double_image_delete(&_di_final);
  double_image_delete(&_di_orig);
}

TEST(ByteImage, Print) {
  char *_s = null;
  ByteImage *_bi = null;

  byte_image_new(&_bi, 1, 1);
  aprint(&_s, 0, byte_image, _bi);
  EXPECT_STREQ(
      "[ByteImage]\n. Width: 1 pixel\n. Height: 1 pixel\n"
      ". YCC: false\n. [Contents]\n. . [Row 0]\n"
      ". . . [0]  A: 0xff  R: 0x00  G: 0x00  B: 0x00\n"
      ". . [/Row 0]\n. [/Contents]\n[/ByteImage]\n",
      _s);
  string_delete(&_s);
  byte_image_delete(&_bi);

  byte_image_new(&_bi, 2, 1);
  _bi->ycc = true;
  aprint(&_s, 0, byte_image, _bi);
  EXPECT_STREQ(
      "[ByteImage]\n. Width: 2 pixels\n. Height: 1 pixel\n"
      ". YCC: true\n. [Contents]\n. . [Row 0]\n"
      ". . . [0]  A: 0xff  R: 0x00  G: 0x00  B: 0x00\n"
      ". . . [1]  A: 0xff  R: 0x00  G: 0x00  B: 0x00\n"
      ". . [/Row 0]\n. [/Contents]\n[/ByteImage]\n",
      _s);
  string_delete(&_s);
  byte_image_delete(&_bi);

  byte_image_new(&_bi, 2, 3);
  aprint(&_s, 0, byte_image, _bi);
  EXPECT_STREQ(
      "[ByteImage]\n. Width: 2 pixels\n. Height: 3 pixels\n"
      ". YCC: false\n. [Contents]\n. . [Row 0]\n"
      ". . . [0]  A: 0xff  R: 0x00  G: 0x00  B: 0x00\n"
      ". . . [1]  A: 0xff  R: 0x00  G: 0x00  B: 0x00\n"
      ". . [/Row 0]\n. . [Row 1]\n"
      ". . . [0]  A: 0xff  R: 0x00  G: 0x00  B: 0x00\n"
      ". . . [1]  A: 0xff  R: 0x00  G: 0x00  B: 0x00\n"
      ". . [/Row 1]\n. . [Row 2]\n"
      ". . . [0]  A: 0xff  R: 0x00  G: 0x00  B: 0x00\n"
      ". . . [1]  A: 0xff  R: 0x00  G: 0x00  B: 0x00\n"
      ". . [/Row 2]\n. [/Contents]\n[/ByteImage]\n",
      _s);
  string_delete(&_s);
  byte_image_delete(&_bi);

  byte_image_new(&_bi, 2, 3);
  byte_image_set_a(_bi, 0, 0, 0x12);
  byte_image_set_r(_bi, 1, 2, 0x34);
  byte_image_set_g(_bi, 1, 0, 0xab);
  byte_image_set_b(_bi, 0, 1, 0xef);
  test_save_load(ByteImage, byte_image, _bi);
  aprint(&_s, 0, byte_image, _bi);
  EXPECT_STREQ(
      "[ByteImage]\n. Width: 2 pixels\n. Height: 3 pixels\n"
      ". YCC: false\n. [Contents]\n. . [Row 0]\n"
      ". . . [0]  A: 0x12  R: 0x00  G: 0x00  B: 0x00\n"
      ". . . [1]  A: 0xff  R: 0x00  G: 0xab  B: 0x00\n"
      ". . [/Row 0]\n. . [Row 1]\n"
      ". . . [0]  A: 0xff  R: 0x00  G: 0x00  B: 0xef\n"
      ". . . [1]  A: 0xff  R: 0x00  G: 0x00  B: 0x00\n"
      ". . [/Row 1]\n. . [Row 2]\n"
      ". . . [0]  A: 0xff  R: 0x00  G: 0x00  B: 0x00\n"
      ". . . [1]  A: 0xff  R: 0x34  G: 0x00  B: 0x00\n"
      ". . [/Row 2]\n. [/Contents]\n[/ByteImage]\n",
      _s);
  string_delete(&_s);
  byte_image_delete(&_bi);
}

TEST(ByteImage, MakeOpaque) {
  ulong i;
  ulong j;
  ByteImage *_bi = null;

  byte_image_new(&_bi, 2, 3);

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      byte_image_set_a(_bi, i, j, i + j);
    }
  }

  byte_image_make_opaque(_bi);

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      EXPECT_ULEQ(255, byte_image_get_a(_bi, i, j));
    }
  }

  byte_image_delete(&_bi);
}

TEST(ByteImage, Clear) {
  ulong i;
  ulong j;
  ByteImagePixel pixel;
  ByteImage *_bi = null;

  byte_image_new(&_bi, 2, 3);

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      byte_image_set_r(_bi, i, j, i + j);
      byte_image_set_g(_bi, i, j, 128 + i - j);
      byte_image_set_b(_bi, i, j, i * j);
      byte_image_set_a(_bi, i, j, 128 - i - j);
    }
  }

  pixel.r = 12;
  pixel.g = 34;
  pixel.b = 56;
  pixel.a = 78;

  byte_image_clear(_bi, pixel);

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      EXPECT_ULEQ(12, byte_image_get_r(_bi, i, j));
      EXPECT_ULEQ(34, byte_image_get_g(_bi, i, j));
      EXPECT_ULEQ(56, byte_image_get_b(_bi, i, j));
      EXPECT_ULEQ(78, byte_image_get_a(_bi, i, j));
    }
  }

  test_save_load(ByteImage, byte_image, _bi);
  byte_image_delete(&_bi);
}

TEST(ByteImage, SetGet) {
  ulong i;
  ulong j;
  ByteImagePixel pixel;
  ByteImage *_bi = null;

  byte_image_new(&_bi, 2, 3);
  byte_image_set_r(_bi, 0, 0, 27);
  byte_image_set_g(_bi, 1, 2, 98);
  byte_image_set_b(_bi, 0, 1, 7);
  byte_image_set_a(_bi, 0, 1, 253);

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      EXPECT_ULEQ(i == 0 && j == 0 ? 27 : 0, byte_image_get_r(_bi, i, j));
      EXPECT_ULEQ(i == 1 && j == 2 ? 98 : 0, byte_image_get_g(_bi, i, j));
      EXPECT_ULEQ(i == 0 && j == 1 ? 7 : 0, byte_image_get_b(_bi, i, j));
      EXPECT_ULEQ(i == 0 && j == 1 ? 253 : 255, byte_image_get_a(_bi, i, j));
      pixel = byte_image_get(_bi, i, j);
      EXPECT_ULEQ(i == 0 && j == 0 ? 27 : 0, pixel.r);
      EXPECT_ULEQ(i == 1 && j == 2 ? 98 : 0, pixel.g);
      EXPECT_ULEQ(i == 0 && j == 1 ? 7 : 0, pixel.b);
      EXPECT_ULEQ(i == 0 && j == 1 ? 253 : 255, pixel.a);
    }
  }

  byte_image_delete(&_bi);

  byte_image_new(&_bi, 2, 3);
  pixel.g = pixel.b = 0;
  pixel.a = 255;
  pixel.r = 27;
  byte_image_set(_bi, 0, 0, pixel);
  pixel.r = 0;
  pixel.g = 98;
  byte_image_set(_bi, 1, 2, pixel);
  pixel.g = 0;
  pixel.b = 7;
  pixel.a = 253;
  byte_image_set(_bi, 0, 1, pixel);

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      EXPECT_ULEQ(i == 0 && j == 0 ? 27 : 0, byte_image_get_r(_bi, i, j));
      EXPECT_ULEQ(i == 1 && j == 2 ? 98 : 0, byte_image_get_g(_bi, i, j));
      EXPECT_ULEQ(i == 0 && j == 1 ? 7 : 0, byte_image_get_b(_bi, i, j));
      EXPECT_ULEQ(i == 0 && j == 1 ? 253 : 255, byte_image_get_a(_bi, i, j));
      pixel = byte_image_get(_bi, i, j);
      EXPECT_ULEQ(i == 0 && j == 0 ? 27 : 0, pixel.r);
      EXPECT_ULEQ(i == 1 && j == 2 ? 98 : 0, pixel.g);
      EXPECT_ULEQ(i == 0 && j == 1 ? 7 : 0, pixel.b);
      EXPECT_ULEQ(i == 0 && j == 1 ? 253 : 255, pixel.a);
    }
  }

  test_save_load(ByteImage, byte_image, _bi);
  byte_image_delete(&_bi);
}

TEST(ByteImage, ContainsTransparency) {
  ByteImage *_bi = null;

  byte_image_new(&_bi, 2, 3);
  EXPECT_FALSE(byte_image_contains_transparency(_bi));
  byte_image_set_a(_bi, 1, 2, 254);
  EXPECT_TRUE(byte_image_contains_transparency(_bi));
  byte_image_delete(&_bi);
}

TEST(ByteImagePixel, Various) {
  ByteImagePixel pixel;
  ByteImagePixel pixel2;

  pixel = byte_image_pixel_from_rgba_hex(0x12345678);
  EXPECT_ULEQ(0x12, pixel.r);
  EXPECT_ULEQ(0x34, pixel.g);
  EXPECT_ULEQ(0x56, pixel.b);
  EXPECT_ULEQ(0x78, pixel.a);

  pixel = byte_image_pixel_opaque_from_rgb_hex(0x123456);
  EXPECT_ULEQ(0x12, pixel.r);
  EXPECT_ULEQ(0x34, pixel.g);
  EXPECT_ULEQ(0x56, pixel.b);
  EXPECT_ULEQ(0xff, pixel.a);

  pixel = byte_image_pixel_opaque_from_grayscale(0x12);
  EXPECT_ULEQ(0x12, pixel.r);
  EXPECT_ULEQ(0x12, pixel.g);
  EXPECT_ULEQ(0x12, pixel.b);
  EXPECT_ULEQ(0xff, pixel.a);

  pixel = byte_image_pixel_opaque_from_rgb_hex_string("123456");
  EXPECT_ULEQ(0x12, pixel.r);
  EXPECT_ULEQ(0x34, pixel.g);
  EXPECT_ULEQ(0x56, pixel.b);
  EXPECT_ULEQ(0xff, pixel.a);

  pixel = byte_image_pixel_opaque_black();
  EXPECT_ULEQ(0, pixel.r);
  EXPECT_ULEQ(0, pixel.g);
  EXPECT_ULEQ(0, pixel.b);
  EXPECT_ULEQ(255, pixel.a);

  pixel.r = 12;
  pixel.g = 34;
  pixel.b = 56;
  pixel.a = 78;
  pixel2 = pixel;

  EXPECT_TRUE(byte_image_pixel_equals(pixel2, pixel));

  pixel = byte_image_pixel_from_hsl(0.3, 0.4, 0.5);
  EXPECT_ULEQ(97, pixel.r);
  EXPECT_ULEQ(179, pixel.g);
  EXPECT_ULEQ(77, pixel.b);
  EXPECT_ULEQ(255, pixel.a);

  EXPECT_ULEQ(0, byte_bound(-1e30));
  EXPECT_ULEQ(0, byte_bound(-123));
  EXPECT_ULEQ(0, byte_bound(-1));
  EXPECT_ULEQ(0, byte_bound(-1e-6));
  EXPECT_ULEQ(0, byte_bound(0));
  EXPECT_ULEQ(0, byte_bound(0.5 - 1e-6));
  EXPECT_ULEQ(1, byte_bound(0.5 + 1e-6));
  EXPECT_ULEQ(254, byte_bound(254.5 - 1e-6));
  EXPECT_ULEQ(255, byte_bound(254.5 + 1e-6));
  EXPECT_ULEQ(255, byte_bound(255));
  EXPECT_ULEQ(255, byte_bound(256));
  EXPECT_ULEQ(255, byte_bound(260));
  EXPECT_ULEQ(255, byte_bound(1000));
  EXPECT_ULEQ(255, byte_bound(1e30));
}

TEST(ByteImagePixel, Ycca) {
  ushort u;
  ByteImagePixel pixel;
  ByteImageYccaPixel ycca;

  /* Test all opaque grays.
   */
  pixel.a = 255;

  for (u = 0; u < 0x100; u++) {
    pixel.r = pixel.g = pixel.b = (byte)u;
    ycca = byte_image_ycca_pixel_from_rgba(pixel);
    EXPECT_ULEQ(u, ycca.y);
    EXPECT_ULEQ(128, ycca.cb);
    EXPECT_ULEQ(128, ycca.cr);
    EXPECT_ULEQ(255, ycca.a);
    pixel = byte_image_pixel_from_ycca(ycca);
    EXPECT_ULEQ(u, pixel.r);
    EXPECT_ULEQ(u, pixel.g);
    EXPECT_ULEQ(u, pixel.b);
    EXPECT_ULEQ(255, pixel.a);
  }

  /* Test all alpha values for mid-gray.
   */
  pixel.r = pixel.g = pixel.b = 128;

  for (u = 0; u < 0x100; u++) {
    pixel.a = (byte)u;
    ycca = byte_image_ycca_pixel_from_rgba(pixel);
    EXPECT_ULEQ(128, ycca.y);
    EXPECT_ULEQ(128, ycca.cb);
    EXPECT_ULEQ(128, ycca.cr);
    EXPECT_ULEQ(u, ycca.a);
    pixel = byte_image_pixel_from_ycca(ycca);
    EXPECT_ULEQ(128, pixel.r);
    EXPECT_ULEQ(128, pixel.g);
    EXPECT_ULEQ(128, pixel.b);
    EXPECT_ULEQ(u, pixel.a);
  }

  /* Test some particular values.
   */
  pixel.a = 255;
  pixel.r = 255;
  pixel.g = pixel.b = 32;
  ycca = byte_image_ycca_pixel_from_rgba(pixel);
  EXPECT_ULEQ(99, ycca.y);
  EXPECT_ULEQ(90, ycca.cb);
  EXPECT_ULEQ(240, ycca.cr);
  pixel = byte_image_pixel_from_ycca(ycca);
  EXPECT_ULEQ(255, pixel.r);
  EXPECT_ULEQ(32, pixel.g);
  EXPECT_ULEQ(32, pixel.b);

  pixel.g = 255;
  pixel.r = pixel.b = 0;
  ycca = byte_image_ycca_pixel_from_rgba(pixel);
  EXPECT_ULEQ(150, ycca.y);
  EXPECT_ULEQ(44, ycca.cb);
  EXPECT_ULEQ(21, ycca.cr);
  pixel = byte_image_pixel_from_ycca(ycca);
  EXPECT_ULEQ(0, pixel.r);
  EXPECT_ULEQ(255, pixel.g);
  EXPECT_ULEQ(1, pixel.b);

  pixel.b = 255;
  pixel.r = pixel.g = 16;
  ycca = byte_image_ycca_pixel_from_rgba(pixel);
  EXPECT_ULEQ(43, ycca.y);
  EXPECT_ULEQ(248, ycca.cb);
  EXPECT_ULEQ(109, ycca.cr);
  pixel = byte_image_pixel_from_ycca(ycca);
  EXPECT_ULEQ(16, pixel.r);
  EXPECT_ULEQ(15, pixel.g);
  EXPECT_ULEQ(255, pixel.b);
}

FINISH
