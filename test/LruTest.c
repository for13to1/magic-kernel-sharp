#include "LruTest.h"

START

TEST(Lru, Simple) {
  Lru *_ = null;

  lru_new(&_, null, null);
  EXPECT_TRUE(lru_is_empty(_));

  lru_add(_, (void *)1);
  EXPECT_FALSE(lru_is_empty(_));
  EXPECT_PEQ((void *)1, lru_pop(_));
  EXPECT_TRUE(lru_is_empty(_));

  lru_add(_, (void *)1);
  lru_add(_, (void *)2);
  EXPECT_FALSE(lru_is_empty(_));
  EXPECT_PEQ((void *)1, lru_pop(_));
  EXPECT_PEQ((void *)2, lru_pop(_));
  EXPECT_TRUE(lru_is_empty(_));

  lru_add(_, (void *)1);
  lru_add(_, (void *)2);
  lru_add(_, (void *)3);
  lru_remove(_, (void *)2);
  EXPECT_PEQ((void *)1, lru_pop(_));
  EXPECT_PEQ((void *)3, lru_pop(_));
  EXPECT_TRUE(lru_is_empty(_));

  lru_add(_, (void *)1);
  lru_add(_, (void *)2);
  lru_add(_, (void *)3);
  lru_touch(_, (void *)2);
  EXPECT_PEQ((void *)1, lru_pop(_));
  EXPECT_PEQ((void *)3, lru_pop(_));
  EXPECT_PEQ((void *)2, lru_pop(_));

  lru_add(_, (void *)1);
  lru_touch(_, (void *)1);
  EXPECT_PEQ((void *)1, lru_pop(_));

  lru_add(_, (void *)1);
  lru_add(_, (void *)2);
  lru_touch(_, (void *)1);
  lru_touch(_, (void *)2);
  EXPECT_PEQ((void *)1, lru_pop(_));
  EXPECT_PEQ((void *)2, lru_pop(_));

  lru_add(_, (void *)1);
  lru_add(_, (void *)2);
  lru_touch(_, (void *)1);
  EXPECT_PEQ((void *)2, lru_pop(_));
  EXPECT_PEQ((void *)1, lru_pop(_));

  lru_add(_, (void *)1);
  lru_add(_, (void *)2);
  lru_add(_, (void *)3);
  lru_touch(_, (void *)1);
  EXPECT_PEQ((void *)2, lru_pop(_));
  EXPECT_PEQ((void *)3, lru_pop(_));
  EXPECT_PEQ((void *)1, lru_pop(_));

  lru_delete(&_);
}

FINISH
