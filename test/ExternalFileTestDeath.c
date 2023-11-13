#include "ExternalFileTestDeath.h"

START_DEATH

TEST(ExternalFile, WriteRead) {
  byte b;
  char *_e = null;
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);

  /* Test that reading a write-only file fails.
   */
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_FALSE(external_file_try_read_byte(_ef, &b));
  string_new_f(
      &_e,
      "Could not read byte from '%s', position 0",
      _tfn->_filename);
  EXPECT_DEATH(external_file_read_byte(_ef), _e);
  string_delete(&_e);
  external_file_delete(&_ef);

  /* Test that writing a read-only file fails.
   */
  external_file_new(&_ef, _tfn->_filename, "r");
  string_new_f(
      &_e,
      "Could not write byte to '%s'",
      _tfn->_filename);
  EXPECT_DEATH(external_file_write_byte(_ef, 1), _e);
  string_delete(&_e);

  /* Test that reading from EOF fails.
   */
  EXPECT_FALSE(external_file_try_read_byte(_ef, &b));
  string_new_f(
      &_e,
      "Could not read byte from '%s', position 0",
      _tfn->_filename);
  EXPECT_DEATH(external_file_read_byte(_ef), _e);
  string_delete(&_e);
  external_file_delete(&_ef);
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, GetSize) {
  char *_e = null;
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, "gz");
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  string_new_f(
      &_e,
      "Do not try to get the size of gzipped file '%s'", _tfn->_filename);
  EXPECT_DEATH(external_file_get_size(_ef), _e);
  string_delete(&_e);
  external_file_delete(&_ef);
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, GetPosition) {
  char *_e = null;
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, "gz");
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  string_new_f(
      &_e,
      "Do not try to get the position of gzipped file '%s'",
      _tfn->_filename);
  EXPECT_DEATH(external_file_get_position(_ef), _e);
  string_delete(&_e);
  external_file_delete(&_ef);
  temp_filename_delete(&_tfn);

  external_file_new(&_ef, null, "stdin");
  EXPECT_DEATH(
      external_file_get_position(_ef),
      "Do not try to get position for (stdin) in text mode 'rt'");
  external_file_delete(&_ef);
}

TEST(ExternalFile, SetPosition) {
  char *_e = null;
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, "gz");
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  string_new_f(
      &_e,
      "Do not try to set the position of gzipped file '%s'",
      _tfn->_filename);
  EXPECT_DEATH(external_file_set_position(_ef, 0), _e);
  string_delete(&_e);
  external_file_delete(&_ef);
  temp_filename_delete(&_tfn);

  external_file_new(&_ef, null, "stdin");
  EXPECT_DEATH(
      external_file_set_position(_ef, 0),
      "Do not try to set position for (stdin) in text mode 'rt'");
  external_file_delete(&_ef);
}

TEST(ExternalFile, AssertContentsEqualsBlob) {
  byte b;
  char *_e = null;
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);
  external_file_new(&_ef, _tfn->_filename, "w+");
  temp_filename_unlock(_tfn);
  b = 0x34;
  string_new_f(&_e, "File '%s' had length 0; expected 1", _tfn->_filename);
  EXPECT_DEATH(
      external_file_assert_contents_equals_blob(_ef, 1, &b),
      _e);
  string_delete(&_e);

  external_file_rewind(_ef);
  external_file_write_byte(_ef, 0x56);
  string_new_f(
      &_e,
      "File '%s' disagreement at offset 0: expected 0x34, actual 0x56",
      _tfn->_filename);
  EXPECT_DEATH(
      external_file_assert_contents_equals_blob(_ef, 1, &b),
      _e);
  string_delete(&_e);

  external_file_rewind(_ef);
  external_file_write_byte(_ef, 0x34);
  external_file_write_byte(_ef, 0x78);
  string_new_f(
      &_e,
      "File '%s' is longer than expected: actual 2, expected 1",
      _tfn->_filename);
  EXPECT_DEATH(
      external_file_assert_contents_equals_blob(_ef, 1, &b),
      _e);
  string_delete(&_e);

  external_file_delete(&_ef);
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, IsEmpty) {
  EXPECT_DEATH(
      external_file_is_empty("/woof"),
      "Could not open '/woof' for reading to check if it is empty");
}

TEST(ExternalFile, Create) {
  char *_e = null;
  TempFilename *_tfn = null;

  EXPECT_DEATH(
      external_file_create("/dev/null/woof"),
      "Could not create file '/dev/null/woof'");

  temp_filename_new(&_tfn, null);
  external_file_create(_tfn->_filename);
  temp_filename_unlock(_tfn);
  string_new_f(&_e, "File '%s' already exists", _tfn->_filename);
  EXPECT_DEATH(external_file_create(_tfn->_filename), _e);
  string_delete(&_e);
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, Remove) {
  EXPECT_DEATH(
      external_file_remove("/woof"),
      "Could not remove file '/woof': No such file or directory");
}

TEST(ExternalFile, Rename) {
  EXPECT_DEATH(
      external_file_rename("/woof", "miaow"),
      "Could not rename file '/woof' to 'miaow': No such file or directory");
}

TEST(BlobNew, ExternalFileContents) {
  EXPECT_DEATH(
      blob_new_external_file_contents_from_filename(null, "/woof"),
      "Could not open file '/woof' in mode 'r'");
}

TEST(ExternalFile, GetAnsifile) {
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, "gz");
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_DEATH(
      external_file_get_ansifile(_ef),
      "Cannot get ANSI FILE from a gzipped ExternalFile");
  external_file_delete(&_ef);
  temp_filename_delete(&_tfn);
}

TEST(AnsifileFromFilename, ContentsEqualsBlob) {
  EXPECT_DEATH(
      ansifile_from_filename_contents_equals_blob("/woof", 0, null),
      "Could not open file '/woof'");
}

FINISH_DEATH
