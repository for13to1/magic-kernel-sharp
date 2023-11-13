/* Value class for a "dressed" pointer, i.e., an object pointer together with a
 * passback object pointer x.
 */
#ifndef DRESSED_H
#define DRESSED_H

#ifdef __cplusplus
extern "C" {
#endif

struct Dressed;

#include "Printer.h"

typedef struct Dressed {
  void *this;
  void *x;
} Dressed;

/* A print function of a dressed pointer that just prints the pointer's
 * address.
 */
void dressed_print(Dressed *d, struct Printer *p);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
