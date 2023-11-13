/* Static library providing some utility methods for the sRGB colorspace.
 */
#ifndef SRGB_H
#define SRGB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jpc.h"

/* Convert between linear [0, 1] double space and byte-valued gamma space.
 */
byte srgb_linear_to_byte_gamma(double v);
double srgb_byte_gamma_to_linear(byte b);

/* Convert between linear and gamma spaces, but remain in the [0, 1] double
 * gamma space. Note that the much faster byte lookup functions above should be
 * used except for special situations.
 */
double srgb_double_linear_to_gamma(double v);
double srgb_double_gamma_to_linear(double v);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
