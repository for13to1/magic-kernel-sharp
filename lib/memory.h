/* Static library providing a simple method for determining the total amount of
 * memory in the system.
 */
#ifndef MEMORY_H
#define MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "TextFile.h"
#include "numbers.h"
#include "spawn.h"

/* The total amount of memory, in bytes.
 */
ulong memory_get_total(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
