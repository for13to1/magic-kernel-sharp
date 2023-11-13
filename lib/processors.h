/* Static library providing simple methods to figure out how many processors we
 * might have available.
 */
#ifndef PROCESSORS_H
#define PROCESSORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "TextFile.h"
#include "memory.h"
#include "numbers.h"
#include "spawn.h"

/* Number of processors in total.
 */
ulong processors_get_total(void);

/* An estimate of the number of processors free, based on one-second
 * utilization.
 */
ulong processors_get_free(void);

/* Total amount of memory per processor, in bytes.
 */
ulong processors_get_memory_per(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
