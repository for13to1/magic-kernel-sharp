#include "RepeatingKernelTestDeath.h"

static KernelPossible possible_(ulong out, void *x);
static KernelPossible possible_bad_(ulong out, void *x);
static double weight_(ulong out, long in, void *x);
static double weight_bad_1_(ulong out, long in, void *x);
static double weight_bad_2_(ulong out, long in, void *x);

START_DEATH

TEST(RepeatingKernel, New) {
  RepeatingKernel *_ = null;

  EXPECT_DEATH(
      repeating_kernel_new(&_, 0, 1, possible_, weight_, null),
      "n_in cannot be zero");
  EXPECT_DEATH(
      repeating_kernel_new(&_, 1, 0, possible_, weight_, null),
      "n_out cannot be zero");
  repeating_kernel_delete(&_);

  EXPECT_DEATH(
      repeating_kernel_new(&_, 1, 1, null, weight_, null),
      "'possible' function pointer cannot be null");
  repeating_kernel_delete(&_);

  EXPECT_DEATH(
      repeating_kernel_new(&_, 1, 1, possible_, null, null),
      "'weight' function pointer cannot be null");
  repeating_kernel_delete(&_);

  EXPECT_DEATH(
      repeating_kernel_new(&_, 1, 1, possible_bad_, weight_, null),
      "Last (-1) cannot be less than first (0) for out = 0");
  repeating_kernel_delete(&_);

  EXPECT_DEATH(
      repeating_kernel_new(&_, 1, 1, possible_, weight_bad_1_, null),
      "Sum of weights (0) for out = 0 was not positive, "
      "which is not supported");
  repeating_kernel_delete(&_);

  EXPECT_DEATH(
      repeating_kernel_new(&_, 1, 1, possible_, weight_bad_2_, null),
      "Sum of weights (-3.4) for out = 0 was not positive, which is "
      "not supported");
  repeating_kernel_delete(&_);
}

TEST(RepeatingKernel, Get) {
  RepeatingKernel *_ = null;

  repeating_kernel_new_identity(&_, 1);
  EXPECT_DEATH(repeating_kernel_get(_, 0, 1), "in (1) must be in [0, 0]");
  EXPECT_DEATH(
      repeating_kernel_get(_, 1, 0),
      "out (1) must be less than n_out (1)");
  EXPECT_DEATH(
      repeating_kernel_get(_, 1, 1),
      "out (1) must be less than n_out (1)");
  EXPECT_DEATH(
      repeating_kernel_get(_, 0, 2),
      "in (2) must be in [0, 0]");
  EXPECT_DEATH(
      repeating_kernel_get(_, 2, 0),
      "out (2) must be less than n_out (1)");
  EXPECT_DEATH(
      repeating_kernel_get(_, 2, 2),
      "out (2) must be less than n_out (1)");
  repeating_kernel_delete(&_);

  repeating_kernel_new_identity(&_, 2);
  EXPECT_DEATH(
      repeating_kernel_get(_, 0, 2),
      "in (2) must be in [0, 0]");
  EXPECT_DEATH(
      repeating_kernel_get(_, 2, 0),
      "out (2) must be less than n_out (2)");
  EXPECT_DEATH(
      repeating_kernel_get(_, 1, 2),
      "in (2) must be in [1, 1]");
  EXPECT_DEATH(
      repeating_kernel_get(_, 2, 2),
      "out (2) must be less than n_out (2)");
  EXPECT_DEATH(
      repeating_kernel_get(_, 0, 3),
      "in (3) must be in [0, 0]");
  EXPECT_DEATH(
      repeating_kernel_get(_, 1, 3),
      "in (3) must be in [1, 1]");
  EXPECT_DEATH(
      repeating_kernel_get(_, 3, 0),
      "out (3) must be less than n_out (2)");
  EXPECT_DEATH(
      repeating_kernel_get(_, 3, 3),
      "out (3) must be less than n_out (2)");
  repeating_kernel_delete(&_);
}

FINISH_DEATH

static KernelPossible possible_(ulong out, void *x) {
  KernelPossible possible;

  possible.first = (long)out;
  possible.last = (long)out;

  return possible;
}

static KernelPossible possible_bad_(ulong out, void *x) {
  KernelPossible possible;

  possible.first = (long)out;
  possible.last = (long)out - 1;

  return possible;
}

static double weight_(ulong out, long in, void *x) {
  return 3.4;
}

static double weight_bad_1_(ulong out, long in, void *x) {
  return 0;
}

static double weight_bad_2_(ulong out, long in, void *x) {
  return -3.4;
}
