#include "LoggerTestDeath.h"

START_DEATH

TEST(Logger, UnfinishedSingle) {
  TempFilename *_tfn = null;
  File *_f = null;
  Logger *_l = null;

  temp_filename_new(&_tfn, "txt");
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  log_timestamps(false);
  logger_new(&_l, ((ExternalFile *)_f->_ef)->ansifile);
  slog(_l, "Testing %d %d %d...", 1, 2, 3);
  EXPECT_DEATH(
      logger_delete(&_l),
      "Logger destructor called with 1 message still not finished: "
      "'Testing 1 2 3...'");
  file_delete(&_f);
  temp_filename_delete(&_tfn);
}

TEST(Logger, UnfinishedDouble) {
  TempFilename *_tfn = null;
  File *_f = null;
  Logger *_l = null;

  temp_filename_new(&_tfn, "txt");
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  log_timestamps(false);
  logger_new(&_l, ((ExternalFile *)_f->_ef)->ansifile);
  slog(_l, "Testing %d %d %d...", 1, 2, 3);
  slog(_l, "Also %d %d %d...", 4, 5, 6);
  EXPECT_DEATH(
      logger_delete(&_l),
      "Logger destructor called with 2 messages still not finished: "
      "'Testing 1 2 3...', 'Also 4 5 6...'");
  file_delete(&_f);
  temp_filename_delete(&_tfn);
}

TEST(Logger, UnfinishedOuterOfTwo) {
  TempFilename *_tfn = null;
  File *_f = null;
  Logger *_l = null;

  temp_filename_new(&_tfn, "txt");
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  log_timestamps(false);
  logger_new(&_l, ((ExternalFile *)_f->_ef)->ansifile);
  slog(_l, "Testing %d %d %d...", 1, 2, 3);
  slog(_l, "Also %d %d %d...", 4, 5, 6);
  flog(_l);
  EXPECT_DEATH(
      logger_delete(&_l),
      "Logger destructor called with 1 message still not finished: "
      "'Testing 1 2 3...'");
  file_delete(&_f);
  temp_filename_delete(&_tfn);
}

TEST(Logger, FinishedNonexistent) {
  TempFilename *_tfn = null;
  File *_f = null;
  Logger *_l = null;

  temp_filename_new(&_tfn, "txt");
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  log_timestamps(false);
  logger_new(&_l, ((ExternalFile *)_f->_ef)->ansifile);
  EXPECT_DEATH(
      flog(_l),
      "flog() called with no slog() messages available");
  file_delete(&_f);
  temp_filename_delete(&_tfn);
}

FINISH_DEATH
