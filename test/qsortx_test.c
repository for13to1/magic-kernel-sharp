#include "qsortx_test.h"
#include <math.h>

#define N_ 10000
#define M_ 10
#define P_ 9
#define Q_ 10
#define K_ 10000

typedef struct {
  int index;
  int compare;
} Info_;

static ulong size_;

static int std_compare_(const void *a, const void *b);

static sbyte imp_compare_(byte *a, byte *b, ulong *x);

static sbyte shuffle_compare_(Info_ *a, Info_ *b, int *compares);

static sbyte test_compare_(int *a, int *b, short *x);

static void print_(ulong n,
    ulong size,
    byte *std,
    byte *imp,
    bool q,
    ulong qu,
    ulong bu,
    ulong bv);


START

TEST(QsortxBsearchx, PassThrough) {
  short x;
  int key;
  int *r;
  int a[] = {24, 9, 1966};

  x = 7;
  qsortx(a, 3, test_compare_, false, &x);
  key = 1966;
  r = (int *)bsearchx(&key, a, 3, test_compare_, &x);
  EXPECT_PEQ(a + 2, r);

  x = -3;
  qsortx(a, 3, test_compare_, false, &x);
  r = (int *)bsearchx(&key, a, 3, test_compare_, &x);
  EXPECT_PEQ(a, r);
}

TEST(QsortxBsearchx, Comprehensive) {
  byte n_width;
  byte size_width;
  byte *_std = null;
  byte *_imp = null;
  ulong i;
  ulong n;
  ulong num_bytes;
  ulong size;
  ulong u;
  ulong v;
  Random *_random = null;

  /* Test against stdlib version with global variable instead of pass-through.
   */
  random_new(&_random);

  for (i = 0; i < N_; i++) {
    n_width = random_modulo(_random, M_);
    n = random_modulo(_random, 1 << n_width);
    size_width = random_modulo(_random, P_);
    size = size_ = 1 + random_modulo(_random, 1 << size_width);
    num_bytes = n * size_;
    new(&_std, num_bytes);
    new(&_imp, num_bytes);

    for (u = 0; u < num_bytes; u++) {
      _std[u] = _imp[u] = random_modulo(_random, 256);
    }

    qsort(_std, n, size, std_compare_);
    qsortxn(_imp, n, size, imp_compare_, false, &size);
    EXPECT_ULEQ(size, size_);

    for (u = 0; u < num_bytes; u++) {
      if (_std[u] != _imp[u]) {
        print_(n, size, _std, _imp, true, u, 0, 0);
      }

      EXPECT_ULEQ(_std[u], _imp[u]);
    }

    for (u = 0; u < n; u++) {
      byte *p = (byte *)bsearchxn(
          &_std[u * size],
          _imp,
          n,
          size_,
          imp_compare_,
          &size);
      EXPECT_ULEQ(size, size_);

      for (v = 0; v < size_; v++) {
        if (_std[u * size + v] !=  p[v]) {
          print_(n, size, _std, _imp, false, 0, u, v);
        }

        EXPECT_ULEQ(_std[u * size + v], p[v]);
      }
    }

    EXPECT_ULEQ(size, size_);
    deletev(&_imp);
    deletev(&_std);
  }

  random_delete(&_random);
}

TEST(Bsearchx, FindIndex) {
  uint d[] = {0x07071968, 0x23031998, 0x24091966, 0x28032001};
  ulong i;
  ulong n;
  uint k;

  n = sizeof(d) / sizeof(*d);
  k = 0x07071968;
  EXPECT_ULEQ(0, bsearchx_find_index(&k, d, n, uint_compare, null));

  k = 0x23031998;
  EXPECT_ULEQ(1, bsearchx_find_index(&k, d, n, uint_compare, null));

  k = 0x24091966;
  EXPECT_ULEQ(2, bsearchx_find_index(&k, d, n, uint_compare, null));

  k = 0x28032001;
  EXPECT_ULEQ(3, bsearchx_find_index(&k, d, n, uint_compare, null));

  k = 0x07071968;
  EXPECT_TRUE(bsearchx_try_find_index(&k, d, n, uint_compare, null, &i));
  EXPECT_ULEQ(0, i);

  k = 0x23031998;
  EXPECT_TRUE(bsearchx_try_find_index(&k, d, n, uint_compare, null, &i));
  EXPECT_ULEQ(1, i);

  k = 0x24091966;
  EXPECT_TRUE(bsearchx_try_find_index(&k, d, n, uint_compare, null, &i));
  EXPECT_ULEQ(2, i);

  k = 0x28032001;
  EXPECT_TRUE(bsearchx_try_find_index(&k, d, n, uint_compare, null, &i));
  EXPECT_ULEQ(3, i);

  k = 0;
  EXPECT_FALSE(bsearchx_try_find_index(&k, d, n, uint_compare, null, &i));
}

TEST(Qsortx, Shuffle) {
  int i;
  int j;
  int n;
  int compares[N_] = {5, 4, 3, 0, 4, 3, 2, 3, 1, 1};
  ulong count;
  ulong k;
  ulong min;
  ulong max;
  ulong **_counts = null;
  double mean;
  double stdev;
  Info_ infos[Q_];

  newm(&_counts, Q_, Q_);

  for (k = 0; k < K_; k++) {
    for (i = 0; i < Q_; i++) {
      infos[i].index = i;
      infos[i].compare = compares[i];
    }

    qsortx(infos, Q_, shuffle_compare_, true, compares);

    for (i = 0; i < Q_; i++) {
      _counts[infos[i].index][i]++;
    }
  }

  for (i = 0; i < Q_; i++) {
    for (j = 0; j < Q_; j++) {
      if ((i == 0 && j == 9) || (i == 3 && j == 0) || (i == 6 && j == 3)) {
        n = 1;
      } else if (((i == 8 || i == 9) && (j == 1 || j == 2))
          || ((i == 1 || i == 4) && (j == 7 || j == 8))) {
        n = 2;
      } else if ((i == 2 || i == 5 || i == 7) && (j == 4 || j == 5 || j == 6)) {
        n = 3;
      } else {
        n = 0;
      }

      if (n == 0) {
        min = max = 0;
      } else {
        mean =(double)K_ / n;
        stdev = sqrt((double)K_ * (n - 1)) / n;

        min = (ulong)round(mean - 5 * stdev);
        max = (ulong)round(mean + 5 * stdev);
      }

      count = _counts[i][j];
      EXPECT_TRUE(count >= min);
      EXPECT_TRUE(count <= max);
    }
  }

  deletemv(&_counts, Q_);
}

FINISH


static sbyte shuffle_compare_(Info_ *a, Info_ *b, int *compares) {
  int ca;
  int cb;
  int index_a;
  int index_b;

  index_a = a->index;
  index_b = b->index;

  ca = compares[index_a];
  cb = compares[index_b];

  return ca < cb ? -1 : ca > cb ? +1 : 0;
}

static int std_compare_(const void *a, const void *b) {
  byte ba;
  byte bb;
  byte *as;
  byte *bs;
  ulong u;

  as = (byte *)a;
  bs = (byte *)b;

  for (u = 0; u < size_; u++) {
    ba = as[u];
    bb = bs[u];

    if (ba < bb) {
      return -1;
    }

    if (ba > bb) {
      return +1;
    }
  }

  return 0;
}

static sbyte imp_compare_(byte *a, byte *b, ulong *x) {
  byte ba;
  byte bb;
  ulong size;
  ulong u;

  size = *x;

  for (u = 0; u < size; u++) {
    ba = a[u];
    bb = b[u];

    if (ba < bb) {
      return -1;
    }

    if (ba > bb) {
      return +1;
    }
  }

  return 0;
}

static void print_(ulong n,
    ulong size,
    byte *std,
    byte *imp,
    bool q,
    ulong qu,
    ulong bu,
    ulong bv) {
  ulong u;
  ulong v;

  if (q) {
    fprintf(stderr,
        "qsortx failure at element %lu, byte %lu\n",
        qu / size,
        qu % size);
  } else {
    fprintf(stderr,
        "bsearchx failure at element %lu, byte %lu\n",
        bu, bv);
  }

  for (u = 0; u < n; u++) {
    fprintf(stderr, "[std %lu]: ", u);

    for (v = 0; v < size; v++) {
      fprintf(stderr, "%02x", std[u * size + v]);
    }

    fprintf(stderr, "\n");
    fprintf(stderr, "[imp %lu]: ", u);

    for (v = 0; v < size; v++) {
      fprintf(stderr, "%02x", imp[u * size + v]);
    }

    fprintf(stderr, "\n");
  }
}

static sbyte test_compare_(int *a, int *b, short *x) {
  int as;
  int bs;
  int ia;
  int ib;
  short s;

  ia = *a;
  ib = *b;
  s = *x;

  as = ia * s;
  bs = ib * s;

  return as < bs ? -1 : as > bs ? +1 : 0;
}
