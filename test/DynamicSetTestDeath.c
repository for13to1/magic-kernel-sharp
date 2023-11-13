#include "DynamicSetTestDeath.h"

static void delete_(short **ps);

static void print_(short *s, Printer *p);


START_DEATH

TEST(DynamicSet, Simple) {
  short s;
  short *_s = null;
  DynamicSet *_ = null;

  dynamic_set_new(&_, delete_, short_compare, print_);

  newo(&_s);
  *_s = s = 24;
  EXPECT_FALSE(dynamic_set_contains(_, _s));
  dynamic_set_insert(_, &_s);
  EXPECT_NULL(_s);
  EXPECT_TRUE(dynamic_set_contains(_, &s));

  newo(&_s);
  *_s = s = 24;
  EXPECT_TRUE(dynamic_set_contains(_, _s));
  EXPECT_DEATH(
      dynamic_set_insert(_, &_s),
      "Repeated object:\n"
      ". [DynamicObject]\n"
      ". . 24\n"
      ". [/DynamicObject]\n");
  deleteo(&_s);

  dynamic_set_delete(&_);
}

FINISH_DEATH


static void delete_(short **ps) {
  deleteo(ps);
}

static void print_(short *s, Printer *p) {
  rprint(p, "%d", *s);
}
