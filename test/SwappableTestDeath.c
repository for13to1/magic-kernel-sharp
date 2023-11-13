#include "SwappableTestDeath.h"

START_DEATH

TEST(SwappableValues, Simple) {
  byte *_a = null;
  byte *_b = null;
  SwappableValues *_ = null;
  SwappableValues *_2 = null;

  EXPECT_DEATH(
      swappable_set_in_memory_limit(0),
      "Swappable in-memory limit cannot be zero bytes");

  swappable_values_new(&_, byte, 10);
  swappable_values_access(_, &_a);
  EXPECT_DEATH(swappable_values_access(_, &_a), "Pointer not empty");
  EXPECT_DEATH(
      swappable_values_delete(&_),
      "Destructor called on swappable values object but 1 hold is "
      "still outstanding");
  swappable_values_access(_, &_b);
  EXPECT_DEATH(
      swappable_values_delete(&_),
      "Destructor called on swappable values object but 2 holds "
      "are still outstanding");
  swappable_values_release(_, &_a);
  EXPECT_DEATH(
      swappable_values_delete(&_),
      "Destructor called on swappable values object but 1 hold is "
      "still outstanding");
  swappable_values_release(_, &_b);
  EXPECT_DEATH(
      swappable_values_release(_, &_b),
      "All holds of this swappable values object have already been released");
  swappable_values_delete(&_);

  swappable_set_in_memory_limit(20);
  swappable_values_new(&_, byte, 10);
  swappable_values_access(_, &_a);
  swappable_values_new(&_2, byte, 11);
  EXPECT_DEATH(
      swappable_values_access(_2, &_b),
      "Cannot allocate swappable 11 B: only 10 B of 20 B not in use");
  swappable_values_delete(&_2);
  swappable_values_release(_, &_a);
  swappable_values_delete(&_);
}

TEST(Swappable, Simple) {
  char *_s1 = null;
  char *_s2 = null;
  Swappable *_swappable = null;

  swappable_new(&_swappable, string);
  EXPECT_DEATH(
      swappable_withdraw(_swappable),
      "No object has been deposited in this swappable");
  EXPECT_DEATH(
      swappable_release(_swappable),
      "No object has been deposited in this swappable");
  EXPECT_DEATH(
      swappable_deposit(_swappable, &_s1),
      "Depositing pointer is empty");
  string_new_copy(&_s1, "woof");
  string_new_copy(&_s2, "miaow");
  swappable_deposit(_swappable, &_s1);
  EXPECT_DEATH(
      swappable_deposit(_swappable, &_s2),
      "An object has already been deposited in this swappable");
  string_delete(&_s2);
  string_new_copy(&_s1, "dog");
  EXPECT_DEATH(
      swappable_withdraw(_swappable),
      "Another object has been created in the pointer for this swappable");
  EXPECT_DEATH(
      swappable_reacquire(_swappable),
      "Another object has been created in the pointer for this swappable");
  string_delete(&_s1);
  swappable_reacquire(_swappable);
  EXPECT_DEATH(
      swappable_withdraw(_swappable),
      "Cannot withdraw object from the swappable: "
      "it has 1 unreleased reacquisition");
  EXPECT_DEATH(
      swappable_delete(&_swappable),
      "Cannot destroy the swappable: it has 1 unreleased reacquisition");
  swappable_reacquire(_swappable);
  EXPECT_DEATH(
      swappable_withdraw(_swappable),
      "Cannot withdraw object from the swappable: "
      "it has 2 unreleased reacquisitions");
  EXPECT_DEATH(
      swappable_delete(&_swappable),
      "Cannot destroy the swappable: it has 2 unreleased reacquisitions");
  swappable_release(_swappable);
  swappable_release(_swappable);
  EXPECT_DEATH(
      swappable_release(_swappable),
      "There are no unreleased reacquisitions for this swappable");
  swappable_withdraw(_swappable);
  string_delete(&_s1);
  swappable_delete(&_swappable);
}

FINISH_DEATH
