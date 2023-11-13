/* Structure object class for converting between doubles and their IEEE 754
 * representation. Should work regardless of endianness. Will fail if doubles
 * are not stored in IEEE 754 format at all, which should be rare; this is
 * checked in the unit tests.
 */
#ifndef IEEE_DOUBLE_H
#define IEEE_DOUBLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bits.h"
#include "jpc.h"
#include "numbers.h"

typedef struct {
  ulong mantissa : 52;
  ulong exponent : 11;
  ulong negative : 1;
} IeeeDouble;

struct Printer;

/* Make an IeeeDouble from a double.
 */
IeeeDouble ieee_double_make(double d);

/* Print.
 */
void ieee_double_print(IeeeDouble this, struct Printer *p);

/* Get a double from an IeeeDouble.
 */
double double_from_ieee_double(IeeeDouble i);

/* Get a double from explicit negative, exponent, and mantissa values.
 */
double double_from_parts(bool negative, ushort exponent, ulong mantissa);

/* Convert an IeeeDouble to a ulong in a standardized IEEE 754 format.
 */
ulong ulong_from_ieee_double(IeeeDouble i);

/* Convert a double into a ulong in a standardized IEEE 754 format.
 */
ulong ulong_from_double(double d);

/* Convert a standardized IEEE 754 format ulong to an IeeeDouble.
 */
IeeeDouble ieee_double_from_ulong(ulong u);

/* Convert a standardized IEEE 754 format ulong to a double.
 */
double double_from_ulong(ulong u);

/* Return the number of significant bits stored in an IeeeDouble. This is
 * zero for positive or negative zero, and in [1, 53] for all other normal
 * numbers. Throws if it is subnormal, infinite, or not a number.
 */
byte ieee_double_significant_bits(IeeeDouble i);

/* Return the number of significant bits stored in a double, according to the
 * above definitions.
 */
byte double_significant_bits(double d);

/* Return an IeeeDouble that is negative zero.
 */
IeeeDouble ieee_double_negative_zero(void);

/* Return a double that is negative zero.
 */
double double_negative_zero(void);

/* Return an IeeeDouble that is positive infinity.
 */
IeeeDouble ieee_double_positive_infinity(void);

/* Return a double that is positive infinity.
 */
double double_positive_infinity(void);

/* Return an IeeeDouble that is negative infinity.
 */
IeeeDouble ieee_double_negative_infinity(void);

/* Return a double that is negative infinity.
 */
double double_negative_infinity(void);

/* Return an IeeeDouble that is not a number.
 */
IeeeDouble ieee_double_nan(void);

/* Return a double that is not a number.
 */
double double_nan(void);

/* Return an IeeeDouble that is an arbitrary subnormal number.
 */
IeeeDouble ieee_double_subnormal(void);

/* Return a double that is an arbitrary subnormal number.
 */
double double_subnormal(void);

/* Whether an IeeeDouble is negative zero.
 */
bool ieee_double_is_negative_zero(IeeeDouble i);

/* Whether a double is negative zero.
 */
bool double_is_negative_zero(double d);

/* Whether an IeeeDouble is positive infinity.
 */
bool ieee_double_is_positive_infinity(IeeeDouble i);

/* Whether a double is positive infinity.
 */
bool double_is_positive_infinity(double d);

/* Whether an IeeeDouble is negative infinity.
 */
bool ieee_double_is_negative_infinity(IeeeDouble i);

/* Whether a double is negative infinity.
 */
bool double_is_negative_infinity(double d);

/* Whether an IeeeDouble is not a number.
 */
bool ieee_double_is_nan(IeeeDouble i);

/* Whether a double is not a number.
 */
bool double_is_nan(double d);

/* Whether an IeeeDouble is subnormal.
 */
bool ieee_double_is_subnormal(IeeeDouble i);

/* Whether a double is subnormal.
 */
bool double_is_subnormal(double d);

/* Whether an IeeeDouble is negative zero or subnormal. This method is more
 * efficient than calling the two methods individually.
 */
bool ieee_double_is_negative_zero_or_subnormal(IeeeDouble i);

/* Whether a double is negative zero or subnormal. This method is more
 * efficient than calling the two methods individually.
 */
bool double_is_negative_zero_or_subnormal(double d);

/* Whether an IeeeDouble is positive or negative infinity or not a number.
 * This method is more efficient than calling the three methods individually.
 */
bool ieee_double_is_infinite_or_nan(IeeeDouble i);

/* Whether a double is positive or negative infinity or not a number. This
 * method is more efficient than calling the three methods individually.
 */
bool double_is_infinite_or_nan(double d);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
