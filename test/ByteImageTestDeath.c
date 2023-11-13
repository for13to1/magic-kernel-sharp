#include "ByteImageTestDeath.h"

START_DEATH

TEST(ByteImage, New) {
  ByteImage *_bi = null;

  EXPECT_DEATH(byte_image_new(&_bi, 0, 1), "Width must be nonzero");
  byte_image_delete(&_bi);

  EXPECT_DEATH(byte_image_new(&_bi, 1, 0), "Height must be nonzero");
  byte_image_delete(&_bi);
}

TEST(ByteImage, NewFromDouble) {
  DoubleImage *_di = null;
  ByteImage *_bi = null;

  double_image_new(&_di, 1, 1, 1);
  _di->gamma = true;
  EXPECT_DEATH(
      byte_image_new_from_double(&_bi, _di),
      "Cannot have YCbCr without 3 channels");
  byte_image_delete(&_bi);
  double_image_delete(&_di);
}

TEST(ByteImage, NewFromDoubleDiff) {
  DoubleImage *_di = null;
  ByteImage *_bi = null;

  double_image_new(&_di, 1, 1, 1);
  EXPECT_DEATH(
      byte_image_new_from_double_diff(&_bi, _di),
      "Byte differences must be computed in gamma space");
  byte_image_delete(&_bi);

  _di->gamma = true;
  EXPECT_DEATH(
      byte_image_new_from_double_diff(&_bi, _di),
      "JPEG-Clear does not support grayscale images");
  byte_image_delete(&_bi);
  double_image_delete(&_di);

  double_image_new(&_di, 1, 1, 4);
  _di->gamma = true;
  EXPECT_DEATH(
      byte_image_new_from_double_diff(&_bi, _di),
      "JPEG-Clear does not support images with an alpha channel");
  byte_image_delete(&_bi);
  double_image_delete(&_di);
}

TEST(ByteImage, Get) {
  ByteImagePixel pixel;
  ByteImage *_bi = null;

  byte_image_new(&_bi, 1, 1);
  EXPECT_DEATH(byte_image_get(_bi, 1, 0), "i (1) must be less than width (1)");
  EXPECT_DEATH(byte_image_get(_bi, 0, 1), "j (1) must be less than height (1)");
  pixel.r = pixel.g = pixel.b = pixel.a = 128;
  EXPECT_DEATH(
      byte_image_set(_bi, 1, 0, pixel),
      "i (1) must be less than width (1)");
  EXPECT_DEATH(
      byte_image_set(_bi, 0, 1, pixel),
      "j (1) must be less than height (1)");
  byte_image_delete(&_bi);
}

TEST(ByteImagePixel, FromRGBHex) {
  EXPECT_DEATH(
      byte_image_pixel_opaque_from_rgb_hex(0x1000000),
      "Hex (0x 100 0000) too large: maximum for RGB is 0x ff ffff");
  EXPECT_DEATH(
      byte_image_pixel_opaque_from_rgb_hex(0xffffffff),
      "Hex (0x ffff ffff) too large: maximum for RGB is 0x ff ffff");
}

TEST(ByteImagePixel, FromRGBHexString) {
  EXPECT_DEATH(
      byte_image_pixel_opaque_from_rgb_hex_string("1000000"),
      "String ('1,000,000') must be in hex format 'rrggbb'");
  EXPECT_DEATH(
      byte_image_pixel_opaque_from_rgb_hex_string("ffffffff"),
      "String ('ffffffff') must be in hex format 'rrggbb'");
  EXPECT_DEATH(
      byte_image_pixel_opaque_from_rgb_hex_string("12345"),
      "String ('12,345') must be in hex format 'rrggbb'");
  EXPECT_DEATH(
      byte_image_pixel_opaque_from_rgb_hex_string("-12345"),
      "Hex value must be between 0x 00 0000 and 0x ff ffff: "
      "read 0x ffff ffff fffe dcbb");
}

FINISH_DEATH
