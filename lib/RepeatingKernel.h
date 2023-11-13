/* Object class for a repeating kernel, which is most useful for
 * one-dimensional resampling by a rational scale factor.
 */
#ifndef REPEATING_KERNEL_H
#define REPEATING_KERNEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "KernelPossible.h"
#include "parallel.h"

typedef struct {
  ulong minParallel;
  ulong nIn;
  ulong nOut;
  ulong numElements;
  long *_firsts;
  long *_lasts;
  double **_weights;
} RepeatingKernel;

/* Constructor, using the same methodology as for the Kernel classl.
 */
void repeating_kernel_new(
    RepeatingKernel **p_this,
    ulong n_in,
    ulong n_out,
    KernelPossible (*possible)(ulong out, void *x),
    double (*weight)(ulong out, long in, void *x),
    void *x);

/* Identity constructor.
 */
void repeating_kernel_new_identity(RepeatingKernel **p_this, ulong n);

/* Destructor.
 */
void repeating_kernel_delete(RepeatingKernel **p_this);

/* Print.
 */
void repeating_kernel_print(RepeatingKernel *this, Printer *p);

/* Apply.
 */
void repeating_kernel_apply(
    RepeatingKernel *this,
    ulong n_out,
    double (*read)(long in, void *x),
    void (*write)(ulong out, double v, void *x),
    void *x);

/* Get a single element.
 */
double repeating_kernel_get(RepeatingKernel *this, ulong out, long in);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
