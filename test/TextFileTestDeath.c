#include "TextFileTestDeath.h"

START_DEATH

TEST(TextFile, New) {
  char *_e = null;
  TempFilename *_tfn = null;
  TextFile *_tf = null;

  temp_filename_new(&_tfn, "txt");
  string_new_f(&_e,
      "Could not open file '%s' in mode 'r'",
      _tfn->_filename);
  EXPECT_DEATH(text_file_new(&_tf, _tfn->_filename, "r"), _e);
  string_delete(&_e);
  temp_filename_delete(&_tfn);

  temp_filename_new(&_tfn, null);
  string_new_f(
      &_e,
      "Could not infer a filetype extension from the filename '%s'",
      _tfn->_filename);
  EXPECT_DEATH(text_file_new(&_tf, _tfn->_filename, "r"), _e);
  string_delete(&_e);
  temp_filename_delete(&_tfn);
}

TEST(TextFile, Read) {
  char **fields;
  TempFilename *_tfn = null;
  File *_f = null;
  TextFile *_tf = null;

  temp_filename_new(&_tfn, "csv");

  file_create(_tfn->_filename);
  temp_filename_unlock(_tfn);
  text_file_new(&_tf, _tfn->_filename, "r");
  EXPECT_DEATH(text_file_read(_tf, &fields), "Couldn't read record");
  EXPECT_DEATH(text_file_read_single(_tf), "Could not read from file");
  text_file_delete(&_tf);

  text_file_new(&_tf, _tfn->_filename, "w");
  text_file_write(_tf, "dog");
  text_file_write(_tf, "cat");
  text_file_end_record(_tf);
  text_file_write(_tf, "dog");
  text_file_end_record(_tf);
  text_file_delete(&_tf);
  text_file_new(&_tf, _tfn->_filename, "r");
  EXPECT_DEATH(
      text_file_read_empty_record(_tf),
      "Record was not empty: it had 2 fields");
  text_file_rewind(_tf);
  EXPECT_DEATH(
      text_file_read_single(_tf),
      "Expected only a single field but read 2 fields");
  EXPECT_DEATH(
      text_file_read_label(_tf, "cat"),
      "Expected to read label 'cat', but read label 'dog'");
  text_file_rewind(_tf);
  EXPECT_DEATH(
      text_file_read_labeled(_tf, "cat"),
      "Expected to read label 'cat', but read label 'dog'");
  text_file_rewind(_tf);
  text_file_read(_tf, &fields);
  text_file_delete(&_tf);

  file_new(&_f, _tfn->_filename, "w");
  file_write_byte(_f, 0x12);
  file_write_byte(_f, 0x00);
  file_write_byte(_f, 0x34);
  file_delete(&_f);
  text_file_new(&_tf, _tfn->_filename, "r");
  EXPECT_DEATH(
      text_file_read(_tf, &fields),
      "Read invalid '\\0' in TextFile");
  text_file_delete(&_tf);

  temp_filename_delete(&_tfn);
}

TEST(TextFile, Write) {
  TempFilename *_tfn = null;
  TextFile *_tf = null;

  temp_filename_new(&_tfn, "csv");
  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_DEATH(
      text_file_write(_tf, ","),
      "Separator character ',' found in field ',' being written");
  EXPECT_DEATH(
      text_file_write(_tf, "\n"),
      "Line terminator character '\\n' found in field '\\n' being written");
  EXPECT_DEATH(
      text_file_write(_tf, "\r"),
      "Line terminator character '\\r' found in field '\\r' being written");
  EXPECT_DEATH(
      text_file_write(_tf, "cat,"),
      "Separator character ',' found in field 'cat,' being written");
  EXPECT_DEATH(
      text_file_write(_tf, "cat\r"),
      "Line terminator character '\\r' found in field 'cat\\r' being written");
  EXPECT_DEATH(
      text_file_write(_tf, "cat\n"),
      "Line terminator character '\\n' found in field 'cat\\n' being written");
  EXPECT_DEATH(
      text_file_write(_tf, ",dog"),
      "Separator character ',' found in field ',dog' being written");
  EXPECT_DEATH(
      text_file_write(_tf, "\ndog"),
      "Line terminator character '\\n' found in field '\\ndog' being written");
  EXPECT_DEATH(
      text_file_write(_tf, "\r\ndog"),
      "Line terminator character '\\r' found in field '\\r\\ndog' "
      "being written");
  EXPECT_DEATH(
      text_file_write(_tf, "cat,dog"),
      "Separator character ',' found in field 'cat,dog' being written");
  EXPECT_DEATH(
      text_file_write(_tf, "cat\ndog"),
      "Line terminator character '\\n' found in field 'cat\\ndog' "
      "being written");
  EXPECT_DEATH(
      text_file_write(_tf, "cat\rdog"),
      "Line terminator character '\\r' found in field 'cat\\rdog' "
      "being written");
  text_file_delete(&_tf);
  temp_filename_delete(&_tfn);

  temp_filename_new(&_tfn, "tsv");
  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_DEATH(
      text_file_write(_tf, "cat\tdog"),
      "Separator character '\\t' found in field 'cat\\tdog' being written");
  text_file_delete(&_tf);
  temp_filename_delete(&_tfn);

  temp_filename_new(&_tfn, "hsv");
  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_DEATH(
      text_file_write(_tf, "cat\001dog"),
      "Separator character '\\001' found in field 'cat\\001dog' being written");
  text_file_delete(&_tf);
  temp_filename_delete(&_tfn);
}

TEST(TextFile, WriteEmptyRecord) {
  TempFilename *_tfn = null;
  TextFile *_tf = null;

  temp_filename_new(&_tfn, "csv");

  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  text_file_write(_tf, "dog");
  EXPECT_DEATH(
      text_file_write_empty_record(_tf),
      "Cannot write empty record from the middle of a record");
  text_file_delete(&_tf);

  temp_filename_delete(&_tfn);
}

FINISH_DEATH
