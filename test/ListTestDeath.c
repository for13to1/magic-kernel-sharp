#include "ListTestDeath.h"

START_DEATH

TEST(List, Simple) {
  void *o;
  List *_ = null;

  EXPECT_DEATH(list_new(&_, null, null, null), "Destructor cannot be null");
  list_delete(&_);

  list_new(&_, delete_nothing, null, null);
  EXPECT_DEATH(list_extract_head(_, &o), "Cannot extract from empty list");
  EXPECT_DEATH(list_extract_tail(_, &o), "Cannot extract from empty list");
  EXPECT_DEATH(list_remove_head(_), "Cannot remove from empty list");
  EXPECT_DEATH(list_remove_tail(_), "Cannot remove from empty list");
  list_delete(&_);
}

FINISH_DEATH
