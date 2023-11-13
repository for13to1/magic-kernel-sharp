#include "RandomTest.h"
#include <float.h>
#include <limits.h>
#include <math.h>

#define N_ 20000000
#define SIGMAS_ 5

static char *version_(
    int n,
    double mn,
    double mx,
    bool normal,
    bool small,
    double (*gen)(Random *random));

static double double_random_modulo_3_(Random *random);

static double double_random_modulo_17_(Random *random);

static double double_random_uint_(Random *random);

static double double_random_ulong_(Random *random);

static double double_random_long_(Random *random);


START

TEST(Random, Modulo3) {
  EXPECT(version_(N_, 0, 2, false, true, double_random_modulo_3_));
}

TEST(Random, Modulo17) {
  EXPECT(version_(N_, 0, 16, false, true, double_random_modulo_17_));
}

TEST(Random, Uint) {
  EXPECT(version_(N_, 0, UINT_MAX, false, false, double_random_uint_));
}

TEST(Random, Ulong) {
  EXPECT(version_(N_,
        0,
        (double)ULONG_MAX, false, false, double_random_ulong_));
}

TEST(Random, Double) {
  Random *_ = null;

  /* Just check that it works. Statistical properties will be trickier.
   */
  random_new(&_);
  random_double(_);
  random_delete(&_);
}

TEST(Random, DoubleUnitCC) {
  EXPECT(version_(N_, 0, 1, false, false, random_double_unit_cc));
}

TEST(Random, DoubleUnitCO) {
  EXPECT(version_(N_, 0, 1, false, false, random_double_unit_co));
}

TEST(Random, DoubleUnitOC) {
  EXPECT(version_(N_, 0, 1, false, false, random_double_unit_oc));
}

TEST(Random, DoubleUnitOO) {
  EXPECT(version_(N_, 0, 1, false, false, random_double_unit_oo));
}

TEST(Random, Long) {
  EXPECT(version_(N_,
        (double)LONG_MIN,
        (double)LONG_MAX,
        false,
        false,
        double_random_long_));
}

FINISH


static char *version_(
    int n,
    double mn,
    double mx,
    bool normal,
    bool small,
    double (*gen)(Random *random)) {
  char *_f = null;
  int i;
  double r;
  double sum;
  double sum_sq;
  double mu;
  double s_sq;
  double e_mu;
  double se_mu;
  double e_s_sq;
  double se_s_sq;
  double z;
  Random *_random = null;

  random_new(&_random);
  sum = sum_sq = 0;

  for (sum = 0, i = 0; i < n; i++) {
    r = gen(_random);

    if (r < mn) {
      string_new_f(&_f, "Returned %.16g but minimum %.16g", r, mn);

      return _f;
    }

    if (r > mx) {
      string_new_f(&_f, "Returned %.16g but maximum %.16g", r, mx);

      return _f;
    }

    sum += r;
    sum_sq += r * r;
  }

  mu = sum / n;
  s_sq = sum_sq / (n - 1) - mu * mu;
  e_s_sq = normal ? 1 : (mx - mn) * (mx - mn) / 12;
  se_s_sq = sqrt(2 * e_s_sq * e_s_sq / (n - 1));
  e_mu = normal ? 0 : (mn + mx) / 2;
  se_mu = sqrt(e_s_sq / n);
  z = (mu - e_mu) / se_mu;

  if (fabs(z) > SIGMAS_) {
    string_new_f(
        &_f,
        "Mean %.16g incorrect (expected %.16g) at %.16g sigmas "
        "(actual %.16g sigmas)",
        mu,
        e_mu,
        (double)SIGMAS_,
        z);

    return _f;
  }

  z = (s_sq - e_s_sq) / se_s_sq;

  if (!small && fabs(z) > SIGMAS_) {
    string_new_f(
        &_f,
        "Standard deviation %.16g incorrect (expected %.16g) at %.16g sigmas "
        "(actual %.16g sigmas)",
        sqrt(s_sq),
        sqrt(e_s_sq),
        (double)SIGMAS_,
        z);

    return _f;
  }

  random_delete(&_random);
  return null;
}

static double double_random_modulo_3_(Random *random) {
  return random_modulo(random, 3);
}

static double double_random_modulo_17_(Random *random) {
  return random_modulo(random, 17);
}

static double double_random_uint_(Random *random) {
  return random_uint(random);
}

static double double_random_ulong_(Random *random) {
  return random_ulong(random);
}

static double double_random_long_(Random *random) {
  return random_long(random);
}
