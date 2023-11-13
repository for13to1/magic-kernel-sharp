#include "jpeg_file_test.h"

START

TEST(JpegFile, Simple) {
  TempFilename *_tfn = null;
  ByteImagePixel pixel;
  ByteImage *_bi = null;

  byte_image_new(&_bi, 2, 2);
  pixel.r = pixel.g = pixel.b = 128;
  byte_image_clear(_bi, pixel);
  byte_image_set_r(_bi, 0, 0, 130);
  byte_image_set_g(_bi, 0, 1, 124);
  byte_image_set_b(_bi, 1, 0, 133);
  temp_filename_new(&_tfn, "jpg");
  byte_image_save_jpeg(_bi, _tfn->_filename, 100);
  temp_filename_unlock(_tfn);
  byte_image_delete(&_bi);
  EXPECT_TRUE(file_is_jpeg(_tfn->_filename));
  byte_image_new_load_jpeg(&_bi, _tfn->_filename);
  EXPECT_ULEQ(129, byte_image_get_r(_bi, 0, 0));
  EXPECT_ULEQ(127, byte_image_get_g(_bi, 0, 0));
  EXPECT_ULEQ(33, (byte_image_get_b(_bi, 0, 0) + 2) >> 2);
  EXPECT_ULEQ(128, byte_image_get_r(_bi, 0, 1));
  EXPECT_ULEQ(126, byte_image_get_g(_bi, 0, 1));
  EXPECT_ULEQ(33, (byte_image_get_b(_bi, 0, 1) + 3) >> 2);
  EXPECT_ULEQ(131, byte_image_get_r(_bi, 1, 0));
  EXPECT_ULEQ(129, byte_image_get_g(_bi, 1, 0));
  EXPECT_ULEQ(33, byte_image_get_b(_bi, 1, 0) >> 2);
  EXPECT_ULEQ(129, byte_image_get_r(_bi, 1, 1));
  EXPECT_ULEQ(127, byte_image_get_g(_bi, 1, 1));
  EXPECT_ULEQ(130, byte_image_get_b(_bi, 1, 1));
  byte_image_delete(&_bi);
  temp_filename_delete(&_tfn);
}

TEST(JpegFile, DefaultQuality) {
  jpeg_file_set_default_quality(47);
  EXPECT_ULEQ(47, jpeg_file_get_default_quality());
  EXPECT_ULEQ(47, jpeg_file_set_default_quality(89));
  EXPECT_ULEQ(89, jpeg_file_get_default_quality());
}

FINISH
