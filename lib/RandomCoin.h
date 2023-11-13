/* Wrapper object class for more efficiently computing random fair coin flips.
 */
#ifndef RANDOM_COIN_H
#define RANDOM_COIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Random.h"

typedef struct {
  byte bitsLeft;
  ulong bits;
  Random *_random;
} RandomCoin;

/* Constructor seeded with the current microsecond.
 */
void random_coin_new(RandomCoin **p_this);

/* Constructor seeded with the given seed.
 */
void random_coin_new_seed(RandomCoin **p_this, uint seed);

/* Constructor seeded with the current microsecond plus the given offset times
 * a random jump.
 */
void random_coin_new_offset(RandomCoin **p_this, uint offset);

/* Destructor.
 */
void random_coin_delete(RandomCoin **p_this);

/* Random coin flip.
 */
bool random_coin_flip(RandomCoin *this);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
