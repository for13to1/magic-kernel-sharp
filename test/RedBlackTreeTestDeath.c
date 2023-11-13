#include "RedBlackTreeTestDeath.h"

static ulong delete_(void **pobj);


START_DEATH

TEST(RedBlackTree, New) {
  RedBlackTree *_tree = null;

  EXPECT_DEATH(
      red_black_tree_new(
        &_tree,
        null,
        short_compare,
        null,
        null),
      "Destructor cannot be null");
  red_black_tree_delete(&_tree);

  EXPECT_DEATH(
      red_black_tree_new(
        &_tree,
        delete_,
        null,
        null,
        null),
      "Comparator cannot be null");
  red_black_tree_delete(&_tree);
}

FINISH_DEATH


static ulong delete_(void **pobj) {
  return 0;
}
