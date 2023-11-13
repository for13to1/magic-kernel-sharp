#include "LruTestDeath.h"

START_DEATH

TEST(Lru, Simple) {
  Lru *_ = null;

  lru_new(&_, null, null);
  EXPECT_DEATH(lru_add(_, null), "Reference cannot be null");
  EXPECT_DEATH(lru_pop(_), "Cannot pop from empty lru");
  lru_delete(&_);
}

FINISH_DEATH
