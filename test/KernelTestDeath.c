#include "KernelTestDeath.h"

static KernelPossible possible_(ulong out, void *x);
static KernelPossible possible_bad_(ulong out, void *x);
static double weight_(ulong out, long in, void *x);
static double weight_bad_1_(ulong out, long in, void *x);
static double weight_bad_2_(ulong out, long in, void *x);

START_DEATH

TEST(Kernel, New) {
  Kernel *_k = null;

  EXPECT_DEATH(
      kernel_new(&_k, 0, 1, false, true, possible_, weight_, null),
      "n_in cannot be zero");
  kernel_delete(&_k);

  EXPECT_DEATH(
      kernel_new(&_k, 1, 0, false, true, possible_, weight_, null),
      "n_out cannot be zero");
  kernel_delete(&_k);

  EXPECT_DEATH(
      kernel_new(&_k, 1, 1, false, true, null, weight_, null),
      "'possible' function pointer cannot be null");
  kernel_delete(&_k);

  EXPECT_DEATH(
      kernel_new(&_k, 1, 1, false, true, possible_, null, null),
      "'weight' function pointer cannot be null");
  kernel_delete(&_k);

  EXPECT_DEATH(
      kernel_new(&_k, 1, 1, false, true, possible_bad_, weight_, null),
      "last (-1) cannot be less than first (0) for out = 0");
  kernel_delete(&_k);

  EXPECT_DEATH(
      kernel_new(&_k, 1, 1, false, true, possible_, weight_bad_1_, null),
      "Did not have any nonzero weights for out = 0");
  kernel_delete(&_k);

  EXPECT_DEATH(
      kernel_new(&_k, 1, 1, false, true, possible_, weight_bad_2_, null),
      "Sum of weights (-3.4) for out = 0 was not positive, which is "
      "not supported");
  kernel_delete(&_k);

  EXPECT_DEATH(kernel_new_identity(&_k, 0), "n_in cannot be zero");
  kernel_delete(&_k);
}

TEST(Kernel, NewCompose) {
  Kernel *_k1 = null;
  Kernel *_k2 = null;
  Kernel *_k3 = null;

  kernel_new_identity(&_k1, 1);
  kernel_new_identity(&_k2, 2);
  EXPECT_DEATH(
      kernel_new_compose(&_k3, _k1, _k2),
      "n_in (2) for the second kernel must equal n_out (1) "
      "of the first kernel");
  kernel_delete(&_k3);
  kernel_delete(&_k2);
  kernel_new_identity(&_k2, 1);
  _k2->periodic = true;
  EXPECT_DEATH(
      kernel_new_compose(&_k3, _k1, _k2),
      "Periodic flag for first kernel (false) must match that of "
      "the second (true)");
  kernel_delete(&_k3);
  kernel_delete(&_k2);
  kernel_delete(&_k1);
}

TEST(Kernel, Get) {
  Kernel *_k = null;

  kernel_new_identity(&_k, 1);
  EXPECT_DEATH(kernel_get(_k, 0, 1), "in (1) must be less than n_in (1)");
  EXPECT_DEATH(kernel_get(_k, 1, 0), "out (1) must be less than n_out (1)");
  EXPECT_DEATH(kernel_get(_k, 1, 1), "in (1) must be less than n_in (1)");
  EXPECT_DEATH(kernel_get(_k, 0, 2), "in (2) must be less than n_in (1)");
  EXPECT_DEATH(kernel_get(_k, 2, 0), "out (2) must be less than n_out (1)");
  EXPECT_DEATH(kernel_get(_k, 2, 2), "in (2) must be less than n_in (1)");
  kernel_delete(&_k);

  kernel_new_identity(&_k, 2);
  EXPECT_DEATH(kernel_get(_k, 0, 2), "in (2) must be less than n_in (2)");
  EXPECT_DEATH(kernel_get(_k, 2, 0), "out (2) must be less than n_out (2)");
  EXPECT_DEATH(kernel_get(_k, 2, 2), "in (2) must be less than n_in (2)");
  EXPECT_DEATH(kernel_get(_k, 0, 3), "in (3) must be less than n_in (2)");
  EXPECT_DEATH(kernel_get(_k, 3, 0), "out (3) must be less than n_out (2)");
  EXPECT_DEATH(kernel_get(_k, 3, 3), "in (3) must be less than n_in (2)");
  kernel_delete(&_k);
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
