#include "DynamicReferenceSetTest.h"

static void print_(short *s, Printer *p);


START

TEST(DynamicReferenceSet, References) {
  void *p;
  DynamicReferenceSet *_ = null;

  dynamic_reference_set_new(&_);
  EXPECT_TRUE(dynamic_reference_set_is_empty(_));

  p = (void *)1;
  EXPECT_FALSE(dynamic_reference_set_contains(_, p));
  dynamic_reference_set_add(_, p);
  EXPECT_FALSE(dynamic_reference_set_is_empty(_));
  EXPECT_TRUE(dynamic_reference_set_contains(_, p));

  dynamic_reference_set_add(_, p);
  EXPECT_TRUE(dynamic_reference_set_contains(_, p));

  p = (void *)2;
  EXPECT_FALSE(dynamic_reference_set_contains(_, p));
  dynamic_reference_set_add(_, p);
  EXPECT_TRUE(dynamic_reference_set_contains(_, p));

  p = (void *)3;
  EXPECT_FALSE(dynamic_reference_set_contains(_, p));
  dynamic_reference_set_add(_, p);
  EXPECT_TRUE(dynamic_reference_set_contains(_, p));

  EXPECT_TRUE(dynamic_reference_set_contains(_, (void *)1));
  EXPECT_TRUE(dynamic_reference_set_contains(_, (void *)2));
  EXPECT_TRUE(dynamic_reference_set_contains(_, (void *)3));
  EXPECT_FALSE(dynamic_reference_set_contains(_, (void *)0));

  dynamic_reference_set_remove(_, (void *)2);
  EXPECT_TRUE(dynamic_reference_set_contains(_, (void *)1));
  EXPECT_FALSE(dynamic_reference_set_contains(_, (void *)2));
  EXPECT_TRUE(dynamic_reference_set_contains(_, (void *)3));
  EXPECT_FALSE(dynamic_reference_set_contains(_, (void *)0));

  dynamic_reference_set_remove(_, (void *)3);
  EXPECT_TRUE(dynamic_reference_set_contains(_, (void *)1));
  EXPECT_FALSE(dynamic_reference_set_contains(_, (void *)2));
  EXPECT_FALSE(dynamic_reference_set_contains(_, (void *)3));
  EXPECT_FALSE(dynamic_reference_set_contains(_, (void *)0));

  dynamic_reference_set_remove(_, (void *)1);
  EXPECT_FALSE(dynamic_reference_set_contains(_, (void *)1));
  EXPECT_FALSE(dynamic_reference_set_contains(_, (void *)2));
  EXPECT_FALSE(dynamic_reference_set_contains(_, (void *)3));
  EXPECT_FALSE(dynamic_reference_set_contains(_, (void *)0));
  EXPECT_TRUE(dynamic_reference_set_is_empty(_));

  dynamic_reference_set_delete(&_);
}

TEST(DynamicReferenceSet, Objects) {
  short s1;
  short s2;
  short s3;
  short s4;
  DynamicReferenceSet *_ = null;

  dynamic_reference_set_new_specify(&_, short_compare, print_);

  s1 = 24;
  EXPECT_FALSE(dynamic_reference_set_contains(_, &s1));
  dynamic_reference_set_add(_, &s1);
  EXPECT_TRUE(dynamic_reference_set_contains(_, &s1));

  dynamic_reference_set_add(_, &s1);
  EXPECT_TRUE(dynamic_reference_set_contains(_, &s1));

  s2 = -9;
  EXPECT_FALSE(dynamic_reference_set_contains(_, &s2));
  dynamic_reference_set_add(_, &s2);
  EXPECT_TRUE(dynamic_reference_set_contains(_, &s2));

  s3 = 1966;
  EXPECT_FALSE(dynamic_reference_set_contains(_, &s3));
  dynamic_reference_set_add(_, &s3);
  EXPECT_TRUE(dynamic_reference_set_contains(_, &s3));

  EXPECT_TRUE(dynamic_reference_set_contains(_, &s1));
  EXPECT_TRUE(dynamic_reference_set_contains(_, &s2));

  s4 = -24;
  EXPECT_FALSE(dynamic_reference_set_contains(_, &s4));

  dynamic_reference_set_delete(&_);
}

FINISH


static void print_(short *s, Printer *p) {
  rprint(p, "%d", *s);
}
