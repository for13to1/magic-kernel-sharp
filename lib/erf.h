/* Static library for the error function.
 */
#ifndef ERF_H
#define ERF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jpc.h"

/* The error function.
 */
double erf(double x);

/* The complementary error function, computed directly, so that there is no
 * loss of precision for large arguments.
 */
double erfc(double x);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
