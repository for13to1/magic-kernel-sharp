#include "shuffle_test.h"

#define N_ 100
#define M_ 15
#define Q_ 100
#define R_ 18


START

TEST(Shuffle, General) {
  byte n_width;
  ulong i;
  ulong n;
  ulong u;
  ulong *_values = null;
  ulong *_orig = null;
  Random *_random = null;

  random_new(&_random);

  for (i = 0; i < N_; i++) {
    n_width = random_modulo(_random, M_);
    n = random_modulo(_random, 1 << n_width);
    new(&_values, n);
    new(&_orig, n);

    for (u = 0; u < n; u++) {
      _orig[u] = _values[u] = random_ulong(_random);
    }

    qsortx(_orig, n, ulong_compare, false, null);
    shuffle(_values, n);
    qsortx(_values, n, ulong_compare, false, null);

    for (u = 0; u < n; u++) {
      EXPECT_ULEQ(_orig[u], _values[u]);
    }

    deletev(&_orig);
    deletev(&_values);
  }

  random_delete(&_random);
}

TEST(Shuffle, Lookups) {
  byte n_width;
  ulong i;
  ulong n;
  ulong u;
  ulong *_lookups = null;
  ulong *_inverses = null;
  Random *_random = null;

  random_new(&_random);

  for (i = 0; i < Q_; i++) {
    n_width = random_modulo(_random, R_);
    n = random_modulo(_random, 1 << n_width);
    shuffle_make_lookups_and_inverses(&_lookups, &_inverses, n);

    for (u = 0; u < n; u++) {
      EXPECT_ULEQ(u, _inverses[_lookups[u]]);
    }

    qsortx(_lookups, n, ulong_compare, false, null);

    for (u = 0; u < n; u++) {
      EXPECT_ULEQ(u, _lookups[u]);
    }

    deletev(&_inverses);
    deletev(&_lookups);
  }
  random_delete(&_random);
}

FINISH
