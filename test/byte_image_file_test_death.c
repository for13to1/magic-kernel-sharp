#include "byte_image_file_test_death.h"

START_DEATH

TEST(ByteImageFile, Death) {
  char *_e = null;
  TempFilename *_tfn = null;
  ByteImage *_bi = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);
  file_new(&_f, _tfn->_filename, "w");
  file_write_string(_f, "This is not really an image file");
  file_delete(&_f);
  string_new_f(&_e,
      "%s cannot be recognized as a PNG or JPEG file",
      _tfn->_filename);
  EXPECT_DEATH(byte_image_new_load_file(&_bi, _tfn->_filename), _e);
  string_delete(&_e);
  temp_filename_delete(&_tfn);

  byte_image_new(&_bi, 1, 1);
  temp_filename_new(&_tfn, null);
  string_new_f(
      &_e,
      "Could not infer the filetype desired from filename '%s'",
      _tfn->_filename);
  EXPECT_DEATH(byte_image_save_file(_bi, _tfn->_filename), _e);
  EXPECT_DEATH(byte_image_save_quality(_bi, _tfn->_filename, 89), _e);
  string_delete(&_e);
  string_new_f(&_e, "Filetype 'dog' not recognized", _tfn->_filename);
  EXPECT_DEATH(
      byte_image_save_type_quality(_bi, _tfn->_filename, "dog", 89),
      _e);
  string_delete(&_e);
  string_delete(&_tfn->_filename);
  byte_image_delete(&_bi);
}

FINISH_DEATH
