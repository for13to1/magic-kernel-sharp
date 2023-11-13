#include "jpeg_file_test_death.h"

START_DEATH

TEST(JpegFile, Death) {
  TempFilename *_tfn = null;
  ByteImage *_bi = null;

  EXPECT_DEATH(
      jpeg_file_set_default_quality(101),
      "Quality (101) must be no greater than 100");

  byte_image_new(&_bi, 2, 2);
  temp_filename_new(&_tfn, "jpg");
  EXPECT_DEATH(
      byte_image_save_jpeg(_bi, _tfn->_filename, 101),
      "Quality (101) must be no greater than 100");
  byte_image_delete(&_bi);
  temp_filename_delete(&_tfn);
}

FINISH_DEATH
