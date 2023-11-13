#include "DynamicSetTest.h"

static void delete_(short **ps);

static void print_(short *s, Printer *p);

static void callback_(short *s, short *sum);


START

TEST(DynamicSet, Simple) {
  short s;
  short *p;
  short *_s = null;
  DynamicSet *_ = null;

  dynamic_set_new(&_, delete_, short_compare, print_);
  EXPECT_TRUE(dynamic_set_is_empty(_));

  newo(&_s);
  *_s = s = 24;
  EXPECT_FALSE(dynamic_set_contains(_, _s));
  EXPECT_NULL(dynamic_set_find(_, &s));
  dynamic_set_insert(_, &_s);
  EXPECT_FALSE(dynamic_set_is_empty(_));
  EXPECT_NULL(_s);
  EXPECT_TRUE(dynamic_set_contains(_, &s));
  p = dynamic_set_find(_, &s);
  EXPECT_ULEQ(s, *p);
  dynamic_set_extract(_, &s, &_s);
  EXPECT_LEQ(s, *_s);
  EXPECT_TRUE(dynamic_set_is_empty(_));
  dynamic_set_insert(_, &_s);
  EXPECT_FALSE(dynamic_set_is_empty(_));
  EXPECT_NULL(_s);
  EXPECT_TRUE(dynamic_set_contains(_, &s));
  p = dynamic_set_find(_, &s);
  EXPECT_ULEQ(s, *p);
  dynamic_set_remove(_, &s);
  EXPECT_TRUE(dynamic_set_is_empty(_));
  newo(&_s);
  *_s = s = 24;
  dynamic_set_insert(_, &_s);

  newo(&_s);
  *_s = s = 9;
  EXPECT_FALSE(dynamic_set_contains(_, _s));
  dynamic_set_insert(_, &_s);
  EXPECT_NULL(_s);
  EXPECT_TRUE(dynamic_set_contains(_, &s));
  p = dynamic_set_find(_, &s);

  newo(&_s);
  *_s = s = 1966;
  EXPECT_FALSE(dynamic_set_contains(_, _s));
  EXPECT_NULL(dynamic_set_find(_, &s));
  dynamic_set_insert(_, &_s);
  EXPECT_NULL(_s);
  EXPECT_TRUE(dynamic_set_contains(_, &s));
  p = dynamic_set_find(_, &s);
  EXPECT_ULEQ(1966, *p);

  s = -24;
  EXPECT_FALSE(dynamic_set_contains(_, &s));
  EXPECT_NULL(dynamic_set_find(_, &s));

  s = 9;
  dynamic_set_remove(_, &s);
  EXPECT_FALSE(dynamic_set_contains(_, &s));

  s = 1966;
  dynamic_set_remove(_, &s);
  EXPECT_FALSE(dynamic_set_contains(_, &s));

  s = 24;
  dynamic_set_remove(_, &s);
  EXPECT_FALSE(dynamic_set_contains(_, &s));

  EXPECT_TRUE(dynamic_set_is_empty(_));

  dynamic_set_delete(&_);
}

TEST(DynamicSet, ForEach) {
  short sum;
  short *_s = null;
  DynamicSet *_ = null;

  dynamic_set_new(&_, delete_, short_compare, print_);
  newo(&_s);
  *_s = 24;
  dynamic_set_insert(_, &_s);
  newo(&_s);
  *_s = 9;
  dynamic_set_insert(_, &_s);
  newo(&_s);
  *_s = -1966;
  dynamic_set_insert(_, &_s);
  sum = 0;
  dynamic_set_foreach(_, callback_, &sum);
  EXPECT_LEQ(-1933, sum);

  dynamic_set_delete(&_);
}

FINISH


static void delete_(short **ps) {
  deleteo(ps);
}

static void print_(short *s, Printer *p) {
  rprint(p, "%d", *s);
}

static void callback_(short *s, short *sum) {
  *sum += *s;
}
