/* Static library for computing MD5 hashes of arbitrary blobs.
 */
#ifndef MD5_H
#define MD5_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jpc.h"

/* Create a new string with the MD5 hash.
 */
void string_new_md5(char **p_this, ulong len, byte *blob);

/* Return the MD5 as two ulongs.
 */
void md5_ulongs(ulong len, byte *blob, ulong *hi, ulong *lo);

/* Write the bytes into an existing array of at least 16 bytes.
 */
void md5_bytes(ulong len, byte *blob, byte *md5);

/* Convert the 16 bytes of an MD5 hash to two ulongs.
 */
void md5_bytes_to_ulongs(byte *md5, ulong *hi, ulong *lo);

/* Create a new string of the MD5 hash from the two ulongs.
 */
void string_new_md5_from_ulongs(char **p_this, ulong hi, ulong lo);

/* Create a new string of the MD5 hash from the 16 bytes.
 */
void string_new_md5_from_bytes(char **p_this, byte *md5);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
