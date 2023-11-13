#include "PrinterTestDeath.h"

START_DEATH

TEST(Printer, Sprint) {
  char *_s = null;
  Printer *_p = null;

  printer_new(&_p, &_s, 0);
  EXPECT_DEATH(
      sprint(_p, "["),
      "Do not use '[', '/', ':', or ']' in sprint heading strings");
  printer_delete(&_p);
  string_delete(&_s);
}

TEST(Printer, UnfinishedSingle) {
  char *_s = null;
  Printer *_p = null;

  printer_new(&_p, &_s, 0);
  sprint(_p, "dog");
  EXPECT_DEATH(
      printer_delete(&_p),
      "Printer destructor called with 1 heading still not finished: 'dog'");
  string_delete(&_s);
}

TEST(Printer, UnfinishedDouble) {
  char *_s = null;
  Printer *_p = null;

  printer_new(&_p, &_s, 0);
  sprint(_p, "dog");
  sprint(_p, "cat");
  EXPECT_DEATH(
      printer_delete(&_p),
      "Printer destructor called with 2 headings still not finished: "
      "'dog', 'cat'");
  string_delete(&_s);
}

TEST(Printer, UnfinishedOuterOfTwo) {
  char *_s = null;
  Printer *_p = null;

  printer_new(&_p, &_s, 0);
  sprint(_p, "dog");
  sprint(_p, "cat");
  fprint(_p);
  EXPECT_DEATH(
      printer_delete(&_p),
      "Printer destructor called with 1 heading still not finished: 'dog'");
  string_delete(&_s);
}

TEST(Printer, FinishNonexistent) {
  char *_s = null;
  Printer *_p = null;

  printer_new(&_p, &_s, 0);
  EXPECT_DEATH(
      fprint(_p),
      "fprint() called with no sprint() headings available");
  printer_delete(&_p);
  string_delete(&_s);
}

TEST(Printer, StackOverflow) {
  byte b;
  char *_s = null;
  Printer *_p = null;

  printer_new(&_p, &_s, 0);

  for (b = 0; b < 100; b++) {
    sprint(_p, "dog");
  }

  EXPECT_DEATH(
      sprint(_p, "dog"),
      "Aborting print: indent level got to 100, which is crazy, "
      "and probably broken");

  for (b = 0; b < 100; b++) {
    fprint(_p);
  }

  printer_delete(&_p);
  string_delete(&_s);
}

FINISH_DEATH
