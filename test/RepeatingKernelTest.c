#include "RepeatingKernelTest.h"
#include <math.h>

typedef struct {
  double *in;
  double *out;
} Info_;

static KernelPossible possible_1_(ulong out, void *x);
static double weight_1_(ulong out, long in, void *x);
static KernelPossible possible_2_(ulong out, void *x);
static double weight_2_(ulong out, long in, void *x);

START

TEST(RepeatingKernel, NewGet) {
  double scale;
  RepeatingKernel *_ = null;

  /* Test small numbers of n_out, checking members, with what is just an
   * identity spec (above).
   */
  repeating_kernel_new(&_, 1, 1, possible_1_, weight_1_, null);
  EXPECT_ULEQ(1, _->nOut);
  EXPECT_DEQ(1, repeating_kernel_get(_, 0, 0));
  repeating_kernel_delete(&_);

  repeating_kernel_new(&_, 2, 2, possible_1_, weight_1_, null);
  EXPECT_ULEQ(2, _->nOut);
  EXPECT_DEQ(1, repeating_kernel_get(_, 0, 0));
  EXPECT_DEQ(1, repeating_kernel_get(_, 1, 1));
  repeating_kernel_delete(&_);

  repeating_kernel_new(&_, 3, 3, possible_1_, weight_1_, null);
  EXPECT_ULEQ(3, _->nOut);
  EXPECT_DEQ(1, repeating_kernel_get(_, 0, 0));
  EXPECT_DEQ(1, repeating_kernel_get(_, 1, 1));
  EXPECT_DEQ(1, repeating_kernel_get(_, 2, 2));
  repeating_kernel_delete(&_);

  /* Now test a kernel that has two taps, and include scaling information in
   * the passthrough x. Start with same-size and the input having enough
   * elements to avoid boundary conditions.
   */
  scale = 1;
  repeating_kernel_new(&_, 2, 2, possible_2_, weight_2_, &scale);
  EXPECT_DEQ(0.25, repeating_kernel_get(_, 0, 0));
  EXPECT_DEQ(0.75, repeating_kernel_get(_, 0, 1));
  EXPECT_DEQ(0.25, repeating_kernel_get(_, 1, 1));
  EXPECT_DEQ(0.75, repeating_kernel_get(_, 1, 2));
  repeating_kernel_delete(&_);

  /* Now change the scale (but with a non-interpolating kernel).
   */
  scale = 2;
  repeating_kernel_new(&_, 4, 4, possible_2_, weight_2_, &scale);
  EXPECT_DEQ(0.25, repeating_kernel_get(_, 0, 0));
  EXPECT_DEQ(0.75, repeating_kernel_get(_, 0, 1));
  EXPECT_DEQ(0.25, repeating_kernel_get(_, 1, 0));
  EXPECT_DEQ(0.75, repeating_kernel_get(_, 1, 1));
  EXPECT_DEQ(0.25, repeating_kernel_get(_, 2, 1));
  EXPECT_DEQ(0.75, repeating_kernel_get(_, 2, 2));
  EXPECT_DEQ(0.25, repeating_kernel_get(_, 3, 1));
  EXPECT_DEQ(0.75, repeating_kernel_get(_, 3, 2));
  repeating_kernel_delete(&_);

  scale = 0.5;
  repeating_kernel_new(&_, 2, 2, possible_2_, weight_2_, &scale);
  EXPECT_DEQ(0.25, repeating_kernel_get(_, 0, 0));
  EXPECT_DEQ(0.75, repeating_kernel_get(_, 0, 1));
  EXPECT_DEQ(0.25, repeating_kernel_get(_, 1, 2));
  EXPECT_DEQ(0.75, repeating_kernel_get(_, 1, 3));
  repeating_kernel_delete(&_);
}

TEST(RepeatingKernal, Print) {
  char *_s = null;
  double scale;
  RepeatingKernel *_ = null;

  scale = 2;
  repeating_kernel_new(&_, 4, 4, possible_2_, weight_2_, &scale);
  aprint(&_s, 0, repeating_kernel, _);
  EXPECT_STREQ(
      "[RepeatingKernel]\n. n_in: 4\n. n_out: 4\n. num_elements: 8\n"
      ". [out = 0]\n. . in = 0: w = 0.25\n. . in = 1: w = 0.75\n. [/out = 0]\n"
      ". [out = 1]\n. . in = 0: w = 0.25\n. . in = 1: w = 0.75\n. [/out = 1]\n"
      ". [out = 2]\n. . in = 1: w = 0.25\n. . in = 2: w = 0.75\n. [/out = 2]\n"
      ". [out = 3]\n. . in = 1: w = 0.25\n. . in = 2: w = 0.75\n. [/out = 3]\n"
      "[/RepeatingKernel]\n",
      _s);
  string_delete(&_s);
  repeating_kernel_delete(&_);
}

FINISH

static KernelPossible possible_1_(ulong out, void *x) {
  KernelPossible possible;

  possible.first = (long)out;
  possible.last = (long)out;

  return possible;
}

static double weight_1_(ulong out, long in, void *x) {
  return 3.4;
}

static KernelPossible possible_2_(ulong out, void *x) {
  double scale;
  KernelPossible possible;

  scale = *(double *)x;

  if (scale >= 1) {
    possible.first = (long)floor((double)out / scale);
    possible.last = (long)ceil((double)out / scale + 1);
  } else {
    possible.first = (long)floor((double)out / scale);
    possible.last = (long)ceil((double)(out + 1) / scale);
  }

  return possible;
}

static double weight_2_(ulong out, long in, void *x) {
  long rel;
  double scale;

  scale = *(double *)x;

  if (scale >= 1) {
    rel = in - (long)floor((double)out / scale);
  } else {
    rel = in - (long)floor((double)out / scale);
  }

  switch (rel) {
    case 0:
      return 1;
    case 1:
      return 3;
    case 2:
      return 0;
    default:
      throw("impossible");
      return 0;
  }
}
