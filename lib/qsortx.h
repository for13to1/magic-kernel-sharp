/* There has been decades of mess there trying to get a passthrough object into
 * qsort() and bsearch(). It isn't in ANSI C, so add it here. Also, as with
 * new() and renew(), these versions also figure out the size of each element
 * for you, avoiding boilerplate error-prone sizeof() copypasta. The
 * comparators also return sbyte rather than int, which avoids some errors when
 * integer arithmetic is attempted instead of direct comparisons, and they
 * automatically cast natural (typed) pointers, rather than forcing them to be
 * void *. This library also automatically executes in parallel when
 * appropriate. Finally, a shuffle flag specifies whether elements that compare
 * equal should be randomly shuffled.
 */
#ifndef QSORTX_H
#define QSORTX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "shuffle.h"

/* See above description.
 */
#define qsortx(base, n, compare, shuffle_equal, x) \
  qsortxI( \
      (base), \
      (n), \
      sizeof(*(base)), \
      (sbyte (*)(void *, void *, void *))(compare), \
      (shuffle_equal), \
      (void *)(x))

void qsortxI(void *base,
    ulong n,
    ulong size,
    sbyte (*compare)(void *a, void *b, void *x),
    bool shuffle_equal,
    void *x);

/* bsearch() with most of the same features described above (other than
 * parallel execution and shuffling, which are not relevant). Note that the key
 * is always passed as the first argument of the comparator, and the array
 * object as the second, which allows an asymmetric implementation: the two
 * objects don't even have to be the same type (but a symmetric comparator, as
 * for qsortx, can always be used if desired).
 */
#define bsearchx(key, base, n, compare, x) \
  bsearchxI( \
      (key), \
      (base), \
      (n), \
      sizeof(*(base)), \
      (sbyte (*)(void *, void *, void *))(compare), \
      (void *)(x))

void *bsearchxI(
    void *key,
    void *base,
    ulong n,
    ulong size,
    sbyte (*compare)(void *key, void *element, void *x),
    void *x);

/* These variants allow you to specify the width of each object in bytes,
 * rather than using the width of the type in the array. Useful when the array
 * is an array of bytes, and your actual objects are wider than one byte.
 */
#define qsortxn(base, n, width, compare, shuffle_equal, x) \
  qsortxI( \
      (base), \
      (n), \
      (width), \
      (sbyte (*)(void *, void *, void *))(compare), \
      (shuffle_equal), \
      (void *)(x))

#define bsearchxn(key, base, n, width, compare, x) \
  bsearchxI( \
      (key), \
      (base), \
      (n), \
      (width), \
      (sbyte (*)(void *, void *, void *))(compare), \
      (void *)(x))

/* Find the index of an element, rather than the pointer to the element.
 */
#define bsearchx_find_index(key, base, n, compare, x) \
  bsearchx_find_indexI( \
      (key), \
      (base), \
      (n), \
      sizeof(*(base)), \
      (sbyte (*)(void *, void *, void *))(compare), \
      (void *)(x))

ulong bsearchx_find_indexI(
    void *key,
    void *base,
    ulong n,
    ulong size,
    sbyte (*compare)(void *key, void *element, void *x),
    void *x);

#define bsearchx_try_find_index(key, base, n, compare, x, pindex) \
  bsearchx_try_find_indexI( \
      (key), \
      (base), \
      (n), \
      sizeof(*(base)), \
      (sbyte (*)(void *, void *, void *))(compare), \
      (void *)(x), \
      (pindex))

bool bsearchx_try_find_indexI(
    void *key,
    void *base,
    ulong n,
    ulong size,
    sbyte (*compare)(void *a, void *b, void *x),
    void *x,
    ulong *pindex);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
