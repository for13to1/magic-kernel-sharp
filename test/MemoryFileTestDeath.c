#include "MemoryFileTestDeath.h"

START_DEATH

TEST(MemoryFile, Basics) {
  MemoryFile *_mf = null;

  memory_file_new(&_mf);

  /* Empty file.
   */
  EXPECT_DEATH(memory_file_read_byte(_mf), "Cannot read byte: end of file");

  /* Write first byte.
   */
  memory_file_write_byte(_mf, 'j');
  EXPECT_DEATH(memory_file_read_byte(_mf), "Cannot read byte: end of file");
  EXPECT_DEATH(memory_file_read_byte(_mf), "Cannot read byte: end of file");
  memory_file_set_position(_mf, 1);

  /* Write second byte.
   */
  EXPECT_DEATH(memory_file_read_byte(_mf), "Cannot read byte: end of file");

  /* Write third byte.
   */
  memory_file_fast_forward(_mf);
  memory_file_write_byte(_mf, 'c');
  EXPECT_DEATH(memory_file_read_byte(_mf), "Cannot read byte: end of file");

  memory_file_delete(&_mf);
}

TEST(MemoryFile, Released) {
  byte b;
  char *_s = null;
  MemoryFile *_mf = null;

  memory_file_new(&_mf);
  memory_file_release(_mf);
  EXPECT_DEATH(
      memory_file_delete(&_mf),
      "Memory file is released; must be reacquired before being destroyed");
  EXPECT_DEATH(
      memory_file_set_position(_mf, 0),
      "No actions can be taken on a released memory file");
  EXPECT_DEATH(
      memory_file_rewind(_mf),
      "No actions can be taken on a released memory file");
  EXPECT_DEATH(
      memory_file_fast_forward(_mf),
      "No actions can be taken on a released memory file");
  EXPECT_DEATH(
      memory_file_write_byte(_mf, 0),
      "No actions can be taken on a released memory file");
  EXPECT_DEATH(
      memory_file_read_byte(_mf),
      "No actions can be taken on a released memory file");
  EXPECT_DEATH(
      memory_file_try_read_byte(_mf, &b),
      "No actions can be taken on a released memory file");
  EXPECT_DEATH(
      memory_file_contents_equals_blob(_mf, 0, &b),
      "No actions can be taken on a released memory file");
  EXPECT_DEATH(
      string_new_memory_file_contents(&_s, _mf),
      "No actions can be taken on a released memory file");
  memory_file_reacquire(_mf);
  memory_file_delete(&_mf);
}

TEST(MemoryFile, AssertContentsEqualsBlob) {
  byte b;
  MemoryFile *_ = null;

  memory_file_new(&_);
  b = 0x34;
  EXPECT_DEATH(
      memory_file_assert_contents_equals_blob(_, 1, &b),
      "Memory file shorter than expected: actual 0, expected 1");

  memory_file_rewind(_);
  memory_file_write_byte(_, 0x56);
  EXPECT_DEATH(
      memory_file_assert_contents_equals_blob(_, 1, &b),
      "Disagreement in memory file at offset 0: actual 0x56, expected 0x34");

  memory_file_rewind(_);
  memory_file_write_byte(_, 0x34);
  memory_file_write_byte(_, 0x78);
  EXPECT_DEATH(
      memory_file_assert_contents_equals_blob(_, 1, &b),
      "Memory file longer than expected: actual 2, expected 1");

  memory_file_delete(&_);
}

TEST(String, NewMemoryFile) {
  char *_s = null;
  MemoryFile *_ = null;

  memory_file_new(&_);
  memory_file_write_byte(_, 0x12);
  memory_file_write_byte(_, 0x00);
  memory_file_write_byte(_, 0x34);
  EXPECT_DEATH(
      string_new_memory_file_contents(&_s, _),
      "Invalid '\\0' found in memory file being read into a string");
  memory_file_delete(&_);
}

FINISH_DEATH
