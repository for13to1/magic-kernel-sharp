#include "log_test.h"

START

TEST(Log, Ansifile) {
  char *_s = null;
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, "txt");
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  log_timestamps(false);
  alog(((ExternalFile *)_f->_ef)->ansifile, 0, "Testing %d %d %d...", 1, 2, 3);
  file_delete(&_f);
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ("Testing 1 2 3...\n", _s);
  string_delete(&_s);
  file_delete(&_f);
  temp_filename_delete(&_tfn);
}

FINISH
