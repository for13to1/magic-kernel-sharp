/* Static class providing utility functions for bits.
 */
#ifndef BITS_H
#define BITS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jpc.h"

/* The width, in bits, of a byte.
 */
byte byte_bit_width(byte u);

/* The width, in bits, of a ushort.
 */
byte ushort_bit_width(ushort u);

/* The width, in bits, of a uint.
 */
byte uint_bit_width(uint u);

/* The width, in bits, of a ulong.
 */
byte ulong_bit_width(ulong u);

/* The width, in hexits (hex digits), of a ulong.
 */
byte ulong_hexit_width(ulong u);

/* Reverse the bits of a byte.
 */
byte byte_reverse_bits(byte b);

/* Reverse the bits of a ushort.
 */
ushort ushort_reverse_bits(ushort u);

/* Reverse the bits of a uint.
 */
uint uint_reverse_bits(uint u);

/* Reverse the bits of a ulong.
 */
ulong ulong_reverse_bits(ulong u);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
