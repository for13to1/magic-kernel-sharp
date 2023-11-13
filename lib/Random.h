/* Object class for a psuedo-random number generator with reasonably good
 * properties.
 */
#ifndef RANDOM_H
#define RANDOM_H

#ifdef __cplusplus
extern "C" {
#endif

struct paw;

#include "datetime.h"
#include "paw.h"

typedef struct {
  uint *_m;
  int i;
  ulong lastModulus;
  ulong lastLimit;
} Random;

/* Constructor seeded with the current microsecond.
 */
void random_new(Random **p_this);

/* Constructor seeded with the given seed.
 */
void random_new_seed(Random **p_this, uint seed);

/* Constructor seeded with the current microsecond plus the given offset times
 * a random jump. Useful when more than one random number generator may be
 * created in the same microsecond.
 */
void random_new_offset(Random **p_this, uint offset);

/* Destructor.
 */
void random_delete(Random **p_this);

/* Random ulong in [0, modulus).
 */
ulong random_modulo(Random *this, ulong modulus);

/* Random paw, where the sign, exponent, and mantissa are all randomly
 * generated uniformly within their ranges.
 */
struct paw random_paw(Random *this);

/* Random double, where the sign, exponent, and mantissa are all randomly
 * generated uniformly within their ranges.
 */
double random_double(Random *this);

/* Random double in the unit interval [0, 1]. ('cc' = 'closed, closed').
 */
double random_double_unit_cc(Random *this);

/* Random double in the unit interval [0, 1). ('co' = 'closed, open').
 */
double random_double_unit_co(Random *this);

/* Random double in the unit interval (0, 1]. ('oc' = 'open, closed').
 */
double random_double_unit_oc(Random *this);

/* Random double in the unit interval (0, 1). ('oo' = 'open, open').
 */
double random_double_unit_oo(Random *this);

/* Random boolean, equally likely to be true or false. See also RandomCoin if
 * you need more than a few of these.
 */
bool random_bool(Random *this);

/* Random uint in the complete range [0, 0xffffffff].
 */
uint random_uint(Random *this);

/* Random ulong in the complete range [0, 0xffffffffffffffff].
 */
ulong random_ulong(Random *this);

/* Random long in the complete range [-0x8000000000000000, 0x7fffffffffffffff].
 */
long random_long(Random *this);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
