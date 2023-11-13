/* Simple static library wrapper around mutex locking for pthreads.
 */
#ifndef LOCK_H
#define LOCK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies are tricky for classes included by jpc.h; define Lock here.
 */
#include <pthread.h>
typedef pthread_mutex_t Lock;
#include "jpc.h"

#define LOCK_INITIALIZER PTHREAD_MUTEX_INITIALIZER

/* Lock a lock.
 */
void lock(Lock *lock);

/* Unlock a lock.
 */
void unlock(Lock *lock);

/* Initialize a lock. This is only needed if it cannot be done using the
 * initializer above.
 */
void lock_initialize(Lock *lock);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
