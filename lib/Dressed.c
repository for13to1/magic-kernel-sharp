#include "Dressed.h"

void dressed_print(Dressed *d, Printer *p) {
  rprint(p, "Pointer: %p", d->this);
}
