#include "byte_image_file_test.h"

START

TEST(ByteImageFile, Simple) {
  TempFilename *_tfn = null;
  ByteImage *_bi = null;

  byte_image_new(&_bi, 1, 1);
  temp_filename_new(&_tfn, "png");
  byte_image_save_file(_bi, _tfn->_filename);
  temp_filename_unlock(_tfn);
  byte_image_delete(&_bi);
  EXPECT_STREQ("png", byte_image_filetype(_tfn->_filename));
  byte_image_new_load_file(&_bi, _tfn->_filename);
  byte_image_delete(&_bi);
  temp_filename_delete(&_tfn);

  byte_image_new(&_bi, 1, 1);
  temp_filename_new(&_tfn, "jpg");
  byte_image_save_file(_bi, _tfn->_filename);
  temp_filename_unlock(_tfn);
  byte_image_delete(&_bi);
  EXPECT_STREQ("jpeg", byte_image_filetype(_tfn->_filename));
  byte_image_new_load_file(&_bi, _tfn->_filename);
  byte_image_delete(&_bi);
  temp_filename_delete(&_tfn);

  byte_image_new(&_bi, 1, 1);
  temp_filename_new(&_tfn, "jpg");
  byte_image_save_quality(_bi, _tfn->_filename, 80);
  temp_filename_unlock(_tfn);
  byte_image_delete(&_bi);
  EXPECT_STREQ("jpeg", byte_image_filetype(_tfn->_filename));
  byte_image_new_load_file(&_bi, _tfn->_filename);
  byte_image_delete(&_bi);
  temp_filename_delete(&_tfn);

  byte_image_new(&_bi, 1, 1);
  temp_filename_new(&_tfn, "jpg");
  byte_image_save_type_quality(_bi, _tfn->_filename, "jpeg", 80);
  temp_filename_unlock(_tfn);
  byte_image_delete(&_bi);
  EXPECT_STREQ("jpeg", byte_image_filetype(_tfn->_filename));
  byte_image_new_load_file(&_bi, _tfn->_filename);
  byte_image_delete(&_bi);
  temp_filename_delete(&_tfn);
}

FINISH
