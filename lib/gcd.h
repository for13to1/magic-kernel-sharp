/* Static library for computing the greatest common divisor.
 */
#ifndef GCD_H
#define GCD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jpc.h"

/* The GCD of a and b.
 */
ulong gcd(ulong a, ulong b);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
