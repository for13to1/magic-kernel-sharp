/* Struct for possible bounds of a kernel.
 */
#ifndef KERNEL_POSSIBLE_H
#define KERNEL_POSSIBLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jpc.h"

typedef struct {
  long first;
  long last;
} KernelPossible;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
