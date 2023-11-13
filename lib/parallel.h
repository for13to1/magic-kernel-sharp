/* Static library providing convenience methods around pthreads.
 */
#ifndef PARALLEL_H
#define PARALLEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "processors.h"

typedef struct {
  ulong numThreads;
  ulong t;
  ulong start;
  ulong end;
  void *x;
} Parallel;

/* Parallel 'for' loop. It is conceptually the same as
 *
 *   for (i = 0; i < n; i++) {
 *     run(...);
 *   }
 *
 * Under the hood, the loop is distributed across a number of threads, in such
 * a way that one processor is always left free, and that nested calls do not
 * spawn more than the maximum number of threads available. 'min' specifies the
 * minimum number of 'i' values before a multi-threaded execution is performed.
 * A run() function will typically do something like
 *
 *   Pass_ *pass;
 *
 *   pass = par->x;
 *
 *   for (i = par->start; i < par->end; i++) {
 *     [process]
 *   }
 *
 * where 'Pass_' is often an internal struct used to pass through information.
 */
void parallel_for(ulong n, ulong min, void (*run)(Parallel *par), void *x);

/* Same, where we split all into parallel threads if possible, with no minimum
 * count per thread.
 */
#define parallel_for_all(n, run, x) parallel_for((n), 0, (run), (x))

/* Compute the number of threads that a call to parallel_for() above would use
 * at the current time. This can be used by multi-pass algorithms that
 * partition the data in a certain way and then merge the results. After
 * setting up with this information, the actual call to parallel_for() can
 * specify num_threads by calling parallel_for_specify().
 */
ulong parallel_num_threads(ulong n, ulong min);

/* Specify the number of threads. Usually only a good idea after calling
 * parallel_num_threads() above.
 */
void parallel_for_specify(
    ulong n,
    ulong num_threads,
    void (*run)(Parallel *par),
    void *x);

/* Run in as many parallel threads as are available, without any predfined
 * 'for' loop.
 */
void parallel_do(void (*run)(Parallel *par),void *x);

/* Override the maximum number of threads, for unit tests.
 */
void parallel_override_max_num_threads(ulong num_threads);

/* Simulate thread resource constraints, for unit testing: one out of every
 * denominator thread creation calls will be randomly simulated to fail.
 */
void parallel_simulate_resource_constraints(ulong denominator);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
