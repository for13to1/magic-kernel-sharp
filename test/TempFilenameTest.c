#include "TempFilenameTest.h"

#define N_ 1000


START

TEST(TempFilename, Simple) {
  TempFilename *_ = null;

  temp_filename_new(&_, null);
  temp_filename_delete(&_);

  temp_filename_new(&_, "dog");
  temp_filename_delete(&_);
}

FINISH
