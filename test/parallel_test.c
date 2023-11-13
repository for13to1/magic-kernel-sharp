#include "parallel_test.h"

#define M_ 16
#define N_ 100
#define P_ 8
#define T_ 12

typedef struct {
  ulong *_starts;
  ulong *_ends;
} Pass_;

static void ranges_(Parallel *par);


START

TEST(Parallel, NumThreads) {
  /* For definiteness in the following.
   */
  parallel_override_max_num_threads(7);

  EXPECT_ULEQ(1, parallel_num_threads(0, 0));
  EXPECT_ULEQ(1, parallel_num_threads(0, 1));
  EXPECT_ULEQ(1, parallel_num_threads(0, 2));

  EXPECT_ULEQ(1, parallel_num_threads(1, 0));
  EXPECT_ULEQ(1, parallel_num_threads(1, 1));
  EXPECT_ULEQ(1, parallel_num_threads(1, 2));

  EXPECT_ULEQ(2, parallel_num_threads(2, 0));
  EXPECT_ULEQ(2, parallel_num_threads(2, 1));
  EXPECT_ULEQ(1, parallel_num_threads(2, 2));
  EXPECT_ULEQ(1, parallel_num_threads(2, 3));
  EXPECT_ULEQ(1, parallel_num_threads(2, 4));

  EXPECT_ULEQ(3, parallel_num_threads(3, 0));
  EXPECT_ULEQ(3, parallel_num_threads(3, 1));
  EXPECT_ULEQ(2, parallel_num_threads(3, 2));
  EXPECT_ULEQ(1, parallel_num_threads(3, 3));
  EXPECT_ULEQ(1, parallel_num_threads(3, 4));
  EXPECT_ULEQ(1, parallel_num_threads(3, 5));
  EXPECT_ULEQ(1, parallel_num_threads(3, 6));

  EXPECT_ULEQ(4, parallel_num_threads(4, 0));
  EXPECT_ULEQ(4, parallel_num_threads(4, 1));
  EXPECT_ULEQ(2, parallel_num_threads(4, 2));
  EXPECT_ULEQ(2, parallel_num_threads(4, 3));
  EXPECT_ULEQ(1, parallel_num_threads(4, 4));
  EXPECT_ULEQ(1, parallel_num_threads(4, 5));
  EXPECT_ULEQ(1, parallel_num_threads(4, 6));

  EXPECT_ULEQ(7, parallel_num_threads(7, 0));
  EXPECT_ULEQ(7, parallel_num_threads(7, 1));
  EXPECT_ULEQ(4, parallel_num_threads(7, 2));
  EXPECT_ULEQ(3, parallel_num_threads(7, 3));
  EXPECT_ULEQ(2, parallel_num_threads(7, 4));
  EXPECT_ULEQ(2, parallel_num_threads(7, 5));
  EXPECT_ULEQ(2, parallel_num_threads(7, 6));
  EXPECT_ULEQ(1, parallel_num_threads(7, 7));
  EXPECT_ULEQ(1, parallel_num_threads(7, 8));

  EXPECT_ULEQ(7, parallel_num_threads(8, 0));
  EXPECT_ULEQ(7, parallel_num_threads(8, 1));
  EXPECT_ULEQ(4, parallel_num_threads(8, 2));
  EXPECT_ULEQ(3, parallel_num_threads(8, 3));
  EXPECT_ULEQ(2, parallel_num_threads(8, 4));
  EXPECT_ULEQ(2, parallel_num_threads(8, 5));
  EXPECT_ULEQ(2, parallel_num_threads(8, 6));
  EXPECT_ULEQ(2, parallel_num_threads(8, 7));
  EXPECT_ULEQ(1, parallel_num_threads(8, 8));
  EXPECT_ULEQ(1, parallel_num_threads(8, 9));

  EXPECT_ULEQ(7, parallel_num_threads(40, 0));
  EXPECT_ULEQ(7, parallel_num_threads(40, 1));
  EXPECT_ULEQ(7, parallel_num_threads(40, 2));
  EXPECT_ULEQ(7, parallel_num_threads(40, 3));
  EXPECT_ULEQ(7, parallel_num_threads(40, 4));
  EXPECT_ULEQ(7, parallel_num_threads(40, 5));
  EXPECT_ULEQ(7, parallel_num_threads(40, 6));
  EXPECT_ULEQ(6, parallel_num_threads(40, 7));
  EXPECT_ULEQ(5, parallel_num_threads(40, 8));
  EXPECT_ULEQ(5, parallel_num_threads(40, 9));
  EXPECT_ULEQ(4, parallel_num_threads(40, 10));
  EXPECT_ULEQ(4, parallel_num_threads(40, 13));
  EXPECT_ULEQ(3, parallel_num_threads(40, 14));
  EXPECT_ULEQ(3, parallel_num_threads(40, 19));
  EXPECT_ULEQ(2, parallel_num_threads(40, 20));
  EXPECT_ULEQ(2, parallel_num_threads(40, 39));
  EXPECT_ULEQ(1, parallel_num_threads(40, 40));
  EXPECT_ULEQ(1, parallel_num_threads(40, 41));
  EXPECT_ULEQ(1, parallel_num_threads(40, 1000));
}

TEST(Parallel, Ranges) {
  byte a;
  Pass_ pass;

  /* Since the move to parallel_for(), we have to extract these a little more
   * creatively.
   */
  pass._starts = null;
  new(&pass._starts, 7);
  pass._ends = null;
  new(&pass._ends, 7);
  parallel_for_specify(1234, 7, ranges_, &pass);
  a = 0;
  EXPECT_ULEQ(0, pass._starts[a]);
  EXPECT_ULEQ(176, pass._ends[a++]);
  EXPECT_ULEQ(176, pass._starts[a]);
  EXPECT_ULEQ(352, pass._ends[a++]);
  EXPECT_ULEQ(352, pass._starts[a]);
  EXPECT_ULEQ(528, pass._ends[a++]);
  EXPECT_ULEQ(528, pass._starts[a]);
  EXPECT_ULEQ(705, pass._ends[a++]);
  EXPECT_ULEQ(705, pass._starts[a]);
  EXPECT_ULEQ(881, pass._ends[a++]);
  EXPECT_ULEQ(881, pass._starts[a]);
  EXPECT_ULEQ(1057, pass._ends[a++]);
  EXPECT_ULEQ(1057, pass._starts[a]);
  EXPECT_ULEQ(1234, pass._ends[a++]);
  deletev(&pass._ends);
  deletev(&pass._starts);
}

TEST(Parallel, Resources) {
  byte *_dst = null;
  byte *_src = null;
  ulong n;
  ulong u;
  Random *_random = null;

  /* Simulate resource constraints, since some operating systems can handle a
   * huge number of thread creations (which makes them impossible to test using
   * actual system calls), and some can't (so we need to test that the library
   * handles the situation correctly).
   */
  parallel_simulate_resource_constraints(17);
  random_new(&_random);
  n = 34567;
  new(&_src, n);
  new(&_dst, n);

  for (u = 0; u < n; u++) {
    _src[u] = random_modulo(_random, 256);
  }

  copy_memory(_src, _dst, n);

  for (u = 0; u < n; u++) {
    EXPECT_ULEQ(_src[u], _dst[u]);
  }

  deletev(&_dst);
  deletev(&_src);
  parallel_simulate_resource_constraints(0);

  random_delete(&_random);
}

TEST(Parallel, Speed) {
  ulong i;

  /* Use this to test setup/teardown speed. Seems around 25 us / thread total.
   */
  for (i = 0; i < N_; i++) {
    copy_memory(null, null, 0);
  }
}

FINISH


static void ranges_(Parallel *par) {
  Pass_ *pass;

  pass = par->x;

  pass->_starts[par->t] = par->start;
  pass->_ends[par->t] = par->end;
}

