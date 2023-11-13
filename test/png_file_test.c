#include "png_file_test.h"

START

TEST(PngFile, Simple) {
  TempFilename *_tfn = null;
  ByteImage *_bi = null;

  byte_image_new(&_bi, 2, 2);
  byte_image_set_r(_bi, 0, 0, 89);
  byte_image_set_g(_bi, 0, 1, 125);
  byte_image_set_b(_bi, 1, 0, 46);
  byte_image_set_a(_bi, 1, 1, 129);
  temp_filename_new(&_tfn, "png");
  byte_image_save_png(_bi, _tfn->_filename);
  temp_filename_unlock(_tfn);
  byte_image_delete(&_bi);
  EXPECT_TRUE(file_is_png(_tfn->_filename));
  byte_image_new_load_png(&_bi, _tfn->_filename);
  EXPECT_ULEQ(89, byte_image_get_r(_bi, 0, 0));
  EXPECT_ULEQ(0, byte_image_get_g(_bi, 0, 0));
  EXPECT_ULEQ(0, byte_image_get_b(_bi, 0, 0));
  EXPECT_ULEQ(255, byte_image_get_a(_bi, 0, 0));
  EXPECT_ULEQ(0, byte_image_get_r(_bi, 0, 1));
  EXPECT_ULEQ(125, byte_image_get_g(_bi, 0, 1));
  EXPECT_ULEQ(0, byte_image_get_b(_bi, 0, 1));
  EXPECT_ULEQ(255, byte_image_get_a(_bi, 0, 1));
  EXPECT_ULEQ(0, byte_image_get_r(_bi, 1, 0));
  EXPECT_ULEQ(0, byte_image_get_g(_bi, 1, 0));
  EXPECT_ULEQ(46, byte_image_get_b(_bi, 1, 0));
  EXPECT_ULEQ(255, byte_image_get_a(_bi, 1, 0));
  EXPECT_ULEQ(0, byte_image_get_r(_bi, 1, 1));
  EXPECT_ULEQ(0, byte_image_get_g(_bi, 1, 1));
  EXPECT_ULEQ(0, byte_image_get_b(_bi, 1, 1));
  EXPECT_ULEQ(129, byte_image_get_a(_bi, 1, 1));
  byte_image_delete(&_bi);
  temp_filename_delete(&_tfn);
}

FINISH
