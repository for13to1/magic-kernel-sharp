#include "qsortx_test_death.h"

static sbyte bad_compare_(void *a, void *b, void *x);


START_DEATH

TEST(Qsortx, BadComparator) {
  char *_e = null;
  uint d[] = {0x07071968, 0x23031998, 0x24091966, 0x28032001};
  ulong n;

  string_new_f(
      &_e,
      "Impossible: b (%p) to left of first (%p); comparator must be bad",
      d - 1,
      d);
  string_renew_add_separators(&_e);
  n = sizeof(d) / sizeof(*d);
  EXPECT_DEATH(qsortx(d, n, bad_compare_, false, null), _e);
  string_delete(&_e);
}

TEST(Qsortxn, ZeroWidth) {
  uint d[] = {0x07071968, 0x23031998, 0x24091966, 0x28032001};
  ulong n;

  n = sizeof(d) / sizeof(*d);
  EXPECT_DEATH(
      qsortxn(d, n, 0, uint_compare, false, null),
      "Size of each element cannot be zero");
}

TEST(Bsearchx, FindIndex) {
  uint d[] = {0x07071968, 0x23031998, 0x24091966, 0x28032001};
  ulong n;
  uint k;

  n = sizeof(d) / sizeof(*d);
  k = 0;
  EXPECT_DEATH(
      bsearchx_find_index(&k, d, n, uint_compare, null),
      "Could not find element");
}

FINISH_DEATH


static sbyte bad_compare_(void *a, void *b, void *x) {
  return -1;
}
