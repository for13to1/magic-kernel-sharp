/* Static library providing methods to randomly shuffle the elements of an
 * array.
 */
#ifndef SHUFFLE_H
#define SHUFFLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Random.h"
#include "qsortx.h"

/* Randomly shuffle n objects.
 */
#define shuffle(base, n) \
  shuffleI((base), (n), sizeof(*(base)))

/* Randomly shuffle n objects of specified byte width.
 */
#define shufflen(base, n, width) \
  shuffleI((base), (n), (width));

void shuffleI(void *base, ulong n, ulong size);

/* Create an array of shuffle lookups, i.e. an array of length n with the
 * integers from 0 through n - 1 in random order, and their inverses. Used in
 * this library and also its unit test.
 */
void shuffle_make_lookups_and_inverses(
    ulong **plookups,
    ulong **pinverses,
    ulong n);

/* Same, without the inverses.
 */
void shuffle_make_lookups(ulong **plookups, ulong n);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
