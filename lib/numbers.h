/* Static library of useful utilities for numbers.
 */
#ifndef NUMBERS_H
#define NUMBERS_H

#ifdef __cplusplus
extern "C" {
#endif

struct paw;

#include "bits.h"
#include "paw.h"

/* Parse a string as a bool, throwing if unable.
 */
bool parse_bool(char *s);

/* Parse a string as a byte, throwing if unable.
 */
byte parse_byte(char *s);

/* Parse a string as a byte in decimal format (so that e.g. "09" is interpreted
 * as 9 rather than an invalid octal number), throwing if unable.
 */
byte parse_byte_decimal(char *s);

/* Parse a string as an sbyte, throwing if unable.
 */
sbyte parse_sbyte(char *s);

/* Parse a string as a ushort, throwing if unable.
 */
ushort parse_ushort(char *s);

/* Parse a string as a short, throwing if unable.
 */
short parse_short(char *s);

/* Parse a string as a uint, throwing if unable.
 */
uint parse_uint(char *s);

/* Parse a string as an int, throwing if unable.
 */
int parse_int(char *s);

/* Parse a string as a ulong, throwing if unable.
 */
ulong parse_ulong(char *s);

/* Parse a string as a long, throwing if unable.
 */
long parse_long(char *s);

/* Parse a string as a paw, throwing if unable.
 */
struct paw parse_paw(char *s);

/* Parse a string as a double, throwing if unable.
 */
double parse_double(char *s);

/* Try to parse a string as a bool.
 */
bool try_parse_bool(bool *b, char *s);

/* Try to parse a string as a byte.
 */
bool try_parse_byte(byte *b, char *s);

/* Try to parse a string as a byte in decimal format (so that e.g. "09" is
 * interpreted as 9 rather than an invalid octal number).
 */
bool try_parse_byte_decimal(byte *b, char *s);

/* Try to parse a string as an sbyte.
 */
bool try_parse_sbyte(sbyte *b, char *s);

/* Try to parse a string as a ushort.
 */
bool try_parse_ushort(ushort *u, char *s);

/* Try to parse a string as a short.
 */
bool try_parse_short(short *h, char *s);

/* Try to parse a string as a uint.
 */
bool try_parse_uint(uint *u, char *s);

/* Try to parse a string as an int.
 */
bool try_parse_int(int *i, char *s);

/* Try to parse a string as a ulong.
 */
bool try_parse_ulong(ulong *u, char *s);

/* Try to parse a string as a long.
 */
bool try_parse_long(long *l, char *s);

/* Try to parse a string as a paw.
 */
bool try_parse_paw(struct paw *p, char *s);

/* Try to parse a string as a double.
 */
bool try_parse_double(double *d, char *s);

/* Print a ulong into a string for serialization.
 */
void string_new_serialized_ulong(char **p_this, ulong u);

/* Print a double into a string, using a standardized IEEE 754 format.
 */
void string_new_serialized_double(char **p_this, double d);

/* For qsortx and bsearchx.
 */
sbyte short_compare(short *a, short *b, void *x);

sbyte int_compare(int *a, int *b, void *x);

sbyte uint_compare(uint *a, uint *b, void *x);

sbyte long_compare(long *a, long *b, void *x);

sbyte ulong_compare(ulong *a, ulong *b, void *x);

sbyte double_compare(double *a, double *b, void *x);

/* Number of decimal digits that a ulong has.
 */
byte ulong_digits(ulong u);

/* For Bear.
 */
ulong gibibytes_to_bytes(double gibibytes);

double bytes_to_gibibytes(ulong bytes);

void string_new_memory(char **p_this, ulong bytes);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
