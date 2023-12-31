#include "normal.h"
#include <math.h>

#define SQRT2_ 1.41421356237309504880
#define TWO_ON_SQRTPI_ 1.12837916709551257390
#define ONE_ON_SQRT_TWO_PI_ 0.398942280401433
#define NORM_CENTRAL_ 0.7

double normal(double x) {
  return exp(-x * x / 2) * ONE_ON_SQRT_TWO_PI_;
}

double norminv(double y) {
  if (y <= 0) {
    return -HUGE_VAL;
  }

  if (y >= 1) {
    return HUGE_VAL;
  }

  return SQRT2_ * erfinv(2 * y - 1);
}

double erfinv(double y) {
  static const double a__[4] = {
    0.886226899, -1.645349621, 0.914624893, -0.140543331};
  static const double b__[4] = {
    -2.118377725, 1.442710462, -0.329097515, 0.012229801};
  static const double c__[4] = {
    -1.970840454, -1.624906493, 3.429567803, 1.641345311};
  static const double d__[2] = {3.543889200, 1.637067800};

  double x;
  double z;

  if (y <= -1) {
    return -HUGE_VAL;
  }

  if (y >= 1) {
    return HUGE_VAL;
  }

  if (fabs(y) <= NORM_CENTRAL_) {
    z = y * y;
    x = y * (((a__[3] * z + a__[2]) * z + a__[1]) * z + a__[0]) /
      ((((b__[3] * z + b__[2]) * z + b__[1]) * z + b__[0]) * z + 1);
  } else if (NORM_CENTRAL_ < y) {
    z = sqrt(-log((1 - y) / 2));
    x = (((c__[3] * z + c__[2]) * z + c__[1]) * z + c__[0]) /
      ((d__[1] * z + d__[0]) * z + 1);
  } else {
    z = sqrt(-log((1 + y) / 2));
    x = -(((c__[3] * z + c__[2]) * z + c__[1]) * z + c__[0]) /
      ((d__[1] * z + d__[0]) * z + 1);
  }

  x = x - (erf(x) - y) / (TWO_ON_SQRTPI_ * exp(-x * x));
  x = x - (erf(x) - y) / (TWO_ON_SQRTPI_ * exp(-x * x));

  return x;
}
