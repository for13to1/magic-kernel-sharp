#include "LoggerTest.h"

START

TEST(Logger, Single) {
  char *_s = null;
  TempFilename *_tfn = null;
  File *_file = null;
  Logger *_l = null;

  temp_filename_new(&_tfn, "txt");
  file_new(&_file, _tfn->_filename, "w");
  log_timestamps(false);

  /* Only the struct is immutable size.
   */
  logger_new(&_l, ((ExternalFile *)_file->_ef)->ansifile);

  /* String is length 13, plus terminator.
   */
  slog(_l, "Testing %d %d %d", 1, 2, 3);

  /* Deletes the string.
   */
  flog(_l);

  /* Expect destructor to return same immutable size.
   */
  logger_delete(&_l);

  file_delete(&_file);
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ("[Testing 1 2 3]\n[/Testing 1 2 3]\n", _s);
  string_delete(&_s);
  file_delete(&_file);

  temp_filename_delete(&_tfn);
}

TEST(Logger, Double) {
  char *_s = null;
  TempFilename *_tfn = null;
  File *_file = null;
  Logger *_l = null;

  temp_filename_new(&_tfn, "txt");
  file_new(&_file, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  log_timestamps(false);

  /* Only the struct is immutable size.
   */
  logger_new(&_l, ((ExternalFile *)_file->_ef)->ansifile);

  /* rlog() doesn't change memory unless spaces() increases the indent.
   */
  rlog(_l, "Random 0.");

  slog(_l, "Testing %d %d %d", 1, 2, 3);
  rlog(_l, "Random 1.");
  slog(_l, "Also %d %d %d", 4, 5, 6);
  rlog(_l, "Random 2.");
  flog(_l);
  rlog(_l, "Random 3.");
  flog(_l);
  rlog(_l, "Random 4.");
  logger_delete(&_l);
  file_delete(&_file);
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ(
      "Random 0.\n"
      "[Testing 1 2 3]\n"
      ". Random 1.\n"
      ". [Also 4 5 6]\n"
      ". . Random 2.\n"
      ". [/Also 4 5 6]\n"
      ". Random 3.\n"
      "[/Testing 1 2 3]\n"
      "Random 4.\n",
      _s);
  string_delete(&_s);
  file_delete(&_file);

  temp_filename_delete(&_tfn);
}

FINISH
