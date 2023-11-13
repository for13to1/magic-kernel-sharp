/* Static library providing the Engineering definition of the sinc function,
 * namely, sin(pi x) / (pi x).
 */
#ifndef SINC_H
#define SINC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jpc.h"

/* sinc(x) = sin(pi x) / (pi x).
 */
double sinc(double x);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
