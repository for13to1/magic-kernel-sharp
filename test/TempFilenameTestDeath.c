#include "TempFilenameTestDeath.h"

START_DEATH

TEST(TempFilename, BadExtension) {
  TempFilename *_ = null;

  EXPECT_DEATH(temp_filename_new(&_, "cat/dog"),
      "Extension 'cat/dog' contains '/', which is not allowed");
}

FINISH_DEATH
