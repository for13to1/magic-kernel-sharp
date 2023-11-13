#include "PrinterTest.h"

START

TEST(Printer, None) {
  char *_s = null;
  Printer *_p = null;

  printer_new(&_p, &_s, 0);
  printer_delete(&_p);
  EXPECT_NULL(_s);
}

TEST(Printer, Single) {
  char *_s = null;
  Printer *_p = null;

  printer_new(&_p, &_s, 0);
  rprint(_p, "Cat %u", 12);
  printer_delete(&_p);
  EXPECT_STREQ("Cat 12\n", _s);
  string_delete(&_s);

  printer_new(&_p, &_s, 1);
  rprint(_p, "Cat %u", 12);
  printer_delete(&_p);
  EXPECT_STREQ(". Cat 12\n", _s);
  string_delete(&_s);

  printer_new(&_p, &_s, 2);
  rprint(_p, "Cat %u", 12);
  printer_delete(&_p);
  EXPECT_STREQ(". . Cat 12\n", _s);
  string_delete(&_s);
}

TEST(Printer, OneHeading) {
  char *_s = null;
  Printer *_p = null;

  printer_new(&_p, &_s, 0);
  sprint(_p, "Cat %u", 12);
  fprint(_p);
  printer_delete(&_p);
  EXPECT_STREQ("[Cat 12]\n[/Cat 12]\n", _s);
  string_delete(&_s);

  printer_new(&_p, &_s, 1);
  sprint(_p, "Cat %u", 12);
  fprint(_p);
  printer_delete(&_p);
  EXPECT_STREQ(". [Cat 12]\n. [/Cat 12]\n", _s);
  string_delete(&_s);

  printer_new(&_p, &_s, 2);
  sprint(_p, "Cat %u", 12);
  fprint(_p);
  printer_delete(&_p);
  EXPECT_STREQ(". . [Cat 12]\n. . [/Cat 12]\n", _s);
  string_delete(&_s);

  printer_new(&_p, &_s, 0);
  sprint(_p, "Cat %u", 12);
  rprint(_p, "Dog %d", 3);
  fprint(_p);
  printer_delete(&_p);
  EXPECT_STREQ("[Cat 12]\n. Dog 3\n[/Cat 12]\n", _s);
  string_delete(&_s);

  printer_new(&_p, &_s, 1);
  sprint(_p, "Cat %u", 12);
  rprint(_p, "Dog %d", 3);
  fprint(_p);
  printer_delete(&_p);
  EXPECT_STREQ(". [Cat 12]\n. . Dog 3\n. [/Cat 12]\n", _s);
  string_delete(&_s);
}

TEST(Printer, TwoHeadings) {
  char *_s = null;
  Printer *_p = null;

  printer_new(&_p, &_s, 0);
  sprint(_p, "Cat %u", 12);
  sprint(_p, "Dog");
  fprint(_p);
  fprint(_p);
  printer_delete(&_p);
  EXPECT_STREQ("[Cat 12]\n. [Dog]\n. [/Dog]\n[/Cat 12]\n", _s);
  string_delete(&_s);

  printer_new(&_p, &_s, 1);
  sprint(_p, "Cat %u", 12);
  sprint(_p, "Dog");
  fprint(_p);
  fprint(_p);
  printer_delete(&_p);
  EXPECT_STREQ(". [Cat 12]\n. . [Dog]\n. . [/Dog]\n. [/Cat 12]\n", _s);
  string_delete(&_s);

  printer_new(&_p, &_s, 1);
  sprint(_p, "Cat %u", 12);
  rprint(_p, "Mouse");
  sprint(_p, "Dog");
  rprint(_p, "Rabbit");
  fprint(_p);
  rprint(_p, "Fox");
  fprint(_p);
  printer_delete(&_p);
  EXPECT_STREQ(
      ". [Cat 12]\n. . Mouse\n. . [Dog]\n. . . Rabbit\n. . [/Dog]\n"
      ". . Fox\n. [/Cat 12]\n", _s);
  string_delete(&_s);
}

FINISH
