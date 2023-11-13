/* Static library of utilities for handling byte values, particularly for
 * images and voxmaps.
 */
#ifndef BYTE_H
#define BYTE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jpc.h"

/* Bound a double to the byte range, round it to the nearest byte value, and
 * cast it to a byte.
 */
byte byte_bound(double v);

/* Byte diff encoding, originally just for JPEG-Clear. For small absolute
 * values, the diff value (when converted back to byte range [0, 255] from
 * double range [0, 1] is represented exactly (plus 128), whereas when that
 * diff approaches +255 or -255, it is quantized so that 3 or 4 values map to
 * the same byte-encoded diff value. See the unit tests for more details.
 */
byte byte_encode_diff(double diff);
double byte_decode_diff(byte by);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
