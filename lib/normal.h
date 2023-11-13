/* Static library providing the standard Normal distribution function and its
 * inverse.
 */
#ifndef NORMAL_H
#define NORMAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "erf.h"

/* The standard Normal function.
 */
double normal(double x);

/* The inverse Normal function.
 */
double norminv(double y);

/* The inverse of the erf function.
 */
double erfinv(double y);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
