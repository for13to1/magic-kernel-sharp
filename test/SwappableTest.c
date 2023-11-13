#include "SwappableTest.h"

#define M_ 300
#define N_ 200
#define P_ 10

static void check_(byte *_expected, byte *_actual, ulong n);


START

TEST(SwappableValues, Simple) {
  byte *_a = null;
  byte *_b = null;
  byte *_z = null;
  ulong u;
  Random *_random = null;
  SwappableValues *_1 = null;
  SwappableValues *_2 = null;

  swappable_set_in_memory_limit(10);
  random_new(&_random);
  new(&_z, 10);

  for (u = 0; u < 10; u++) {
    _z[u] = random_modulo(_random, 256);
  }

  random_delete(&_random);

  swappable_values_new(&_1, byte, 10);
  swappable_values_access(_1, &_a);
  copy_memory(_z, _a, 10);
  swappable_values_release(_1, &_a);

  swappable_values_new(&_2, byte, 5);
  swappable_values_access(_2, &_a);
  copy_memory(_z, _a, 5);
  swappable_values_release(_2, &_a);
  swappable_values_access(_2, &_a);
  check_(_z, _a, 5);
  swappable_values_access(_2, &_b);
  check_(_z, _b, 5);
  swappable_values_release(_2, &_a);
  swappable_values_release(_2, &_b);

  swappable_values_access(_1, &_a);
  check_(_z, _a, 10);
  swappable_values_release(_1, &_a);

  swappable_values_delete(&_1);

  swappable_values_access(_2, &_a);
  check_(_z, _a, 5);
  swappable_values_access(_2, &_b);
  check_(_z, _b, 5);
  swappable_values_release(_2, &_a);
  swappable_values_release(_2, &_b);

  swappable_values_delete(&_2);
  deletev(&_z);
}

TEST(Swappable, SwapStatus) {
  uint *_a = null;
  SwappableValues *_1 = null;
  SwappableValues *_2 = null;
  SwappableValues *_3 = null;

  swappable_set_in_memory_limit(250);

  swappable_values_new(&_1, uint, 30);
  EXPECT_ULEQ(120, _1->numBytes);
  EXPECT_TRUE(swappable_values_is_unaccessed(_1));
  EXPECT_FALSE(swappable_values_is_in_memory(_1));
  EXPECT_FALSE(swappable_values_is_in_storage(_1));
  swappable_values_access(_1, &_a);
  EXPECT_FALSE(swappable_values_is_unaccessed(_1));
  EXPECT_TRUE(swappable_values_is_in_memory(_1));
  EXPECT_FALSE(swappable_values_is_in_storage(_1));
  swappable_values_release(_1, &_a);
  EXPECT_FALSE(swappable_values_is_unaccessed(_1));
  EXPECT_TRUE(swappable_values_is_in_memory(_1));
  EXPECT_FALSE(swappable_values_is_in_storage(_1));

  swappable_values_new(&_2, byte, 60);
  EXPECT_ULEQ(60, _2->numBytes);
  EXPECT_TRUE(swappable_values_is_unaccessed(_2));
  EXPECT_FALSE(swappable_values_is_in_memory(_2));
  EXPECT_FALSE(swappable_values_is_in_storage(_2));
  swappable_values_access(_2, &_a);
  EXPECT_FALSE(swappable_values_is_unaccessed(_2));
  EXPECT_TRUE(swappable_values_is_in_memory(_2));
  EXPECT_FALSE(swappable_values_is_in_storage(_2));
  swappable_values_release(_2, &_a);
  EXPECT_FALSE(swappable_values_is_unaccessed(_2));
  EXPECT_TRUE(swappable_values_is_in_memory(_2));
  EXPECT_FALSE(swappable_values_is_in_storage(_2));

  swappable_values_new(&_3, double, 11);
  EXPECT_ULEQ(88, _3->numBytes);
  EXPECT_TRUE(swappable_values_is_unaccessed(_3));
  EXPECT_FALSE(swappable_values_is_in_memory(_3));
  EXPECT_FALSE(swappable_values_is_in_storage(_3));
  EXPECT_FALSE(swappable_values_is_unaccessed(_1));
  EXPECT_TRUE(swappable_values_is_in_memory(_1));
  EXPECT_FALSE(swappable_values_is_in_storage(_1));
  EXPECT_FALSE(swappable_values_is_unaccessed(_2));
  EXPECT_TRUE(swappable_values_is_in_memory(_2));
  EXPECT_FALSE(swappable_values_is_in_storage(_2));
  swappable_values_access(_3, &_a);
  EXPECT_FALSE(swappable_values_is_unaccessed(_3));
  EXPECT_TRUE(swappable_values_is_in_memory(_3));
  EXPECT_FALSE(swappable_values_is_in_storage(_3));
  EXPECT_FALSE(swappable_values_is_unaccessed(_1));
  EXPECT_FALSE(swappable_values_is_in_memory(_1));
  EXPECT_TRUE(swappable_values_is_in_storage(_1));
  EXPECT_FALSE(swappable_values_is_unaccessed(_2));
  EXPECT_TRUE(swappable_values_is_in_memory(_2));
  EXPECT_FALSE(swappable_values_is_in_storage(_2));
  swappable_values_release(_3, &_a);

  swappable_values_delete(&_3);
  swappable_values_delete(&_2);
  swappable_values_delete(&_1);
}

TEST(Swappable, Comprehensive) {
  ulong i2;
  ulong i;
  ulong iteration;
  ulong j;
  ulong si2;
  ulong si;
  ulong t;
  ulong *_column = null;
  ulong *_lookups = null;
  ulong **_m = null;
  long serial;
  long *_serials = null;
  ListNode *node;
  Lru *_lru = null;
  Random *_random = null;
  SwappableValues *other_swappable;
  SwappableValues *swappable;
  SwappableValues **_swappables = null;

  swappable_set_in_memory_limit(M_ * N_ * sizeof(ulong) / 2);
  random_new(&_random);
  newm(&_m, M_, N_);

  for (i = 0; i < M_; i++) {
    for (j = 0; j < N_; j++) {
      _m[i][j] = random_ulong(_random);
    }
  }

  new(&_swappables, M_);
  new(&_serials, M_);
  lru_new(&_lru, null, null);

  for (i = 0; i < M_; i++) {
    swappable_values_new(&_swappables[i], ulong, N_);
    _serials[i] = -1;
    lru_add(_lru, _swappables[i]);
  }

  for (iteration = 0, serial = 0; iteration < P_; iteration++) {
    shuffle_make_lookups(&_lookups, M_);

    for (si = 0; si < M_; si++) {
      i = _lookups[si];
      swappable_values_access(_swappables[i], &_column);
      lru_touch(_lru, _swappables[i]);

      for (j = 0; j < N_; j++) {
        if (iteration == 0) {
          _column[j] = _m[i][j] = random_ulong(_random);
        } else {
          EXPECT_ULEQ(_m[i][j], _column[j]);
        }
      }

      for (si2 = 0; si2 < M_; si2++) {
        i2 = _lookups[si2];
        swappable = _swappables[i2];

        if (iteration == 0) {
          if (si2 < si) {
            if (si >= M_ / 2 && si2 <= si - M_ / 2) {
              EXPECT_TRUE(swappable_values_is_in_storage(swappable));
            } else {
              EXPECT_TRUE(swappable_values_is_in_memory(swappable));
            }
          } else if (si2 == si) {
            EXPECT_TRUE(swappable_values_is_in_memory(swappable));
          } else {
            EXPECT_TRUE(swappable_values_is_unaccessed(swappable));
          }
        } else {
          for (node = ((List *)_lru->_list)->_nil->_next, t = 0;
              node != ((List *)_lru->_list)->_nil;
              node = node ->_next, t++) {
            other_swappable = *(SwappableValues **)node->_object;

            if (t < M_ / 2) {
              EXPECT_TRUE(swappable_values_is_in_memory(other_swappable));
            } else {
              EXPECT_TRUE(swappable_values_is_in_storage(other_swappable));
            }
          }
        }
      }

      swappable_values_release(_swappables[i], &_column);
    }

    deletev(&_lookups);
  }

  lru_delete(&_lru);
  deletev(&_serials);
  delete(&_swappables, M_, swappable_values_delete);
  random_delete(&_random);
  deletemv(&_m, M_);
}


TEST(SwappableValues, Simple) {
  char *e = "dog";
  char *_s = null;
  Swappable *_swappable = null;

  string_new_copy(&_s, e);
  swappable_new_deposit(&_swappable, string, &_s);
  EXPECT_NULL(_s);
  swappable_reacquire(_swappable);
  EXPECT_STREQ(e, _s);
  swappable_reacquire(_swappable);
  EXPECT_STREQ(e, _s);
  swappable_reacquire(_swappable);
  EXPECT_STREQ(e, _s);
  swappable_release(_swappable);
  EXPECT_STREQ(e, _s);
  swappable_release(_swappable);
  EXPECT_STREQ(e, _s);
  swappable_release(_swappable);
  EXPECT_NULL(_s);
  swappable_withdraw(_swappable);
  EXPECT_STREQ(e, _s);
  string_delete(&_s);
  swappable_delete(&_swappable);
}

FINISH


static void check_(byte *_expected, byte *_actual, ulong n) {
  ulong u;

  for (u = 0; u < n; u++) {
    EXPECT_ULEQ(_expected[u], _actual[u]);
  }
}
