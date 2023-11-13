#include "FileTestDeath.h"

#define N_ (12345)

static byte hash_(ulong u);

START_DEATH

TEST(File, New) {
  char *_e = null;
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);

  EXPECT_DEATH(
      file_new(&_f, null, "dog"),
      "Mode for null filename must be 'stdin', 'stdout' or 'stderr'");
  file_delete(&_f);

  string_new_f(
      &_e,
      "Unknown mode character 'c' in mode string 'cat' opening '%s'",
      _tfn->_filename);
  EXPECT_DEATH(file_new(&_f, _tfn->_filename, "cat"), _e);
  string_delete(&_e);
  file_delete(&_f);

  string_new_f(
      &_e,
      "Text mode not supported for non-wrapped ExternalFile "
      "(mode 'rt' file '%s')",
      _tfn->_filename);
  EXPECT_DEATH(file_new(&_f, _tfn->_filename, "rt"), _e);
  string_delete(&_e);
  file_delete(&_f);

  string_new_f(&_e,
      "Could not open file '%s' in mode 'rr'",
      _tfn->_filename);
  EXPECT_DEATH(file_new(&_f, _tfn->_filename, "rr"), _e);
  string_delete(&_e);
  file_delete(&_f);

  string_new_f(
      &_e,
      "Could not open external file '%s' in mode 'rr'",
      _tfn->_filename);
  EXPECT_DEATH(file_new_external(&_f, _tfn->_filename, "rr"), _e);
  string_delete(&_e);
  file_delete(&_f);

  string_new_f(&_e,
      "Could not open file '%s' in mode 'rw'",
      _tfn->_filename);
  EXPECT_DEATH(file_new(&_f, _tfn->_filename, "rw"), _e);
  string_delete(&_e);
  file_delete(&_f);

  string_new_f(&_e,
      "Could not open file '%s' in mode 'br'",
      _tfn->_filename);
  EXPECT_DEATH(file_new(&_f, _tfn->_filename, "br"), _e);
  string_delete(&_e);
  file_delete(&_f);

  string_new_f(&_e,
      "Could not open file '%s' in mode 'r++'",
      _tfn->_filename);
  EXPECT_DEATH(file_new(&_f, _tfn->_filename, "r++"), _e);
  string_delete(&_e);
  file_delete(&_f);

  string_new_f(&_e,
      "Could not open file '%s' in mode '+r'",
      _tfn->_filename);
  EXPECT_DEATH(file_new(&_f, _tfn->_filename, "+r"), _e);
  string_delete(&_e);
  file_delete(&_f);

  string_new_f(
      &_e,
      "Cannot include both 'a' and 'r' in mode (mode 'ar' file '%s')",
      _tfn->_filename);
  string_delete(&_e);
  file_delete(&_f);

  string_new_f(
      &_e,
      "Cannot include both 'a' and 'r' in mode (mode 'ar' file '%s')",
      _tfn->_filename);
  EXPECT_DEATH(file_new(&_f, _tfn->_filename, "ar"), _e);
  string_delete(&_e);
  file_delete(&_f);

  string_new_f(
      &_e,
      "Cannot include both 'a' and 'r' in mode (mode 'ra' file '%s')",
      _tfn->_filename);
  EXPECT_DEATH(file_new(&_f, _tfn->_filename, "ra"), _e);
  string_delete(&_e);
  file_delete(&_f);

  string_new_f(
      &_e,
      "Cannot include both 'a' and 'w' in mode (mode 'aw' file '%s')",
      _tfn->_filename);
  EXPECT_DEATH(file_new(&_f, _tfn->_filename, "aw"), _e);
  string_delete(&_e);
  file_delete(&_f);

  string_new_f(
      &_e,
      "Cannot include both 'a' and 'w' in mode (mode 'wa' file '%s')",
      _tfn->_filename);
  EXPECT_DEATH(file_new(&_f, _tfn->_filename, "wa"), _e);
  string_delete(&_e);
  file_delete(&_f);

  temp_filename_delete(&_tfn);
}

TEST(File, BackupOneByte) {
  File *_f = null;

  file_new_memory(&_f);
  EXPECT_DEATH(
      file_backup_one_byte(_f),
      "Cannot backup one byte when file is at position 0");
  file_write_string(_f, "woof");
  file_rewind(_f);
  EXPECT_DEATH(
      file_backup_one_byte(_f),
      "Cannot backup one byte when file is at position 0");
  file_delete(&_f);
}

TEST(File, GetAnsifile) {
  File *_f = null;

  file_new_memory(&_f);
  EXPECT_DEATH(
      file_get_ansifile(_f),
      "Cannot get ansifile for a memory file");
  file_delete(&_f);
}

TEST(File, AssertContentsEqualsBlob) {
  byte b;
  byte *_data = null;
  char *_e = null;
  ulong u;
  TempFilename *_tfn = null;
  File *_f = null;

  /* Memory file.
   */
  new(&_data, N_);
  file_new_memory(&_f);

  for (u = 0; u < N_; u++) {
    file_write_byte(_f, _data[u] = hash_(u));
  }

  b = _data[123];
  _data[123] = 0;
  EXPECT_DEATH(
      file_assert_contents_equals_blob(_f, N_, _data),
      "Disagreement in memory file at offset 123: actual 0xf1, expected 0x00");
  file_delete(&_f);

  /* External file.
   */
  temp_filename_new(&_tfn, null);
  file_new_external(&_f, _tfn->_filename, "w+");

  _data[123] = b;

  for (u = 0; u < N_; u++) {
    file_write_byte(_f, _data[u]);
  }

  _data[123] = 0;
  string_new_f(
      &_e,
      "File '%s' disagreement at offset 123: expected 0x00, actual 0xf1",
      _tfn->_filename);
  EXPECT_DEATH(file_assert_contents_equals_blob(_f, N_, _data), _e);
  string_delete(&_e);
  file_delete(&_f);
  deletev(&_data);
  temp_filename_delete(&_tfn);
}

TEST(File, VerifyLabel) {
  File *_ = null;

  file_new_memory(&_);
  save_label("dog", _);
  file_rewind(_);
  EXPECT_DEATH(
      load_label("woof", _),
      "File corrupted: expected label 'woof' but loaded a label length of 3 "
      "rather than 4");
  file_delete(&_);

  file_new_memory(&_);
  save_label("dog", _);
  file_rewind(_);
  EXPECT_DEATH(
      load_label("cat", _),
      "File corrupted: read label 'dog' instead of expected 'cat'");
  file_delete(&_);
}

FINISH_DEATH

static byte hash_(ulong u) {
  byte b[16];
  union {
    ulong u;
    byte b[8];
  } converter;

  converter.u = u;
  md5_bytes(8, converter.b, b);
  return b[0];
}
