/* Value class for floating point numbers built specifically for use by Bear.
 * Very similar to Google Brain's bfloat16 but with 7 exponent bits and 8
 * mantissa bits, rather than Google's 8 and 7: I don't need numbers between
 * 10^19 and 10^38, I have no legacy 'float' implementation, and I am not
 * optimizing matrix multiplication in hardware like they are, so I can keep an
 * extra bit of mantissa and still keep 16-bit cardinality efficiency.
 *
 * I could backronym "PAW" as Precision Adjusted Workingvalue, but it's really
 * just something that a bear uses.
 */
#ifndef PAW_H
#define PAW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Printer.h"

#define PAW_MAX (paw_from_parts(0, 126, 0xff))
#define PAW_MIN (paw_from_parts(0, 1, 0x0))

typedef struct paw {
  ushort u;
} paw;

/* Make a paw from a double or float.
 */
paw paw_make(double d);

/* Get a paw from explicit negative, exponent, and mantissa values.
 */
paw paw_from_parts(bool negative, byte exponent, byte mantissa);

/* Print.
 */
void paw_print(paw this, struct Printer *p);

/* Equals.
 */
bool paw_equals(paw this, paw other);

/* Convert a paw to a double.
 */
double double_from_paw(paw p);

/* Return a paw that is zero.
 */
paw paw_zero(void);

/* Return a paw that is one.
 */
paw paw_one(void);

/* Return a paw that is one-half.
 */
paw paw_half(void);

/* Return a paw that is positive infinity.
 */
paw paw_positive_infinity(void);

/* Return a paw that is negative infinity.
 */
paw paw_negative_infinity(void);

/* Return a paw that is not a number.
 */
paw paw_nan(void);

/* Whether a paw is zero.
 */
bool paw_is_zero(paw p);

/* Whether a paw is positive infinity.
 */
bool paw_is_positive_infinity(paw p);

/* Whether a paw is negative infinity.
 */
bool paw_is_negative_infinity(paw p);

/* Whether a paw is not a number.
 */
bool paw_is_nan(paw p);

/* Whether a paw is positive or negative infinity or not a number. This method
 * is more efficient than calling the three methods individually.
 */
bool paw_is_infinite_or_nan(paw p);

/* Negate.
 */
paw paw_negate(paw p);

/* Add.
 */
paw paw_add(paw a, paw b);

/* Subtract.
 */
paw paw_subtract(paw a, paw b);

/* Multiply.
 */
paw paw_multiply(paw a, paw b);

/* Divide.
 */
paw paw_divide(paw a, paw b);

/* Bound.
 */
paw paw_bound(paw x, paw min, paw max);

/* Greater than.
 */
bool paw_gt(paw this, paw other);

/* Less than.
 */
bool paw_lt(paw this, paw other);

/* Greater than or equal to.
 */
bool paw_ge(paw this, paw other);

/* Less than or equal to.
 */
bool paw_le(paw this, paw other);

/* Minimum.
 */
paw paw_min(paw a, paw b);

/* Maximum.
 */
paw paw_max(paw a, paw b);

/* Comparator.
 */
sbyte paw_compare(paw *a, paw *b, void *x);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
