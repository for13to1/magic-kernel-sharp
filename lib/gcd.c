#include "gcd.h"

ulong gcd(ulong a, ulong b) {
  if (a == 0) {
    return b;
  }

  return gcd(b % a, a);
}
