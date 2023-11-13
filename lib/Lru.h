/* Object class for tracking the least recently used of a set of references.
 */
#ifndef LRU_H
#define LRU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "DynamicSet.h"
#include "List.h"

typedef struct {
  void *x;
  void (*print)(void *o, struct Printer *p);
  struct DynamicSet *_set;
  struct List *_list;
} Lru;

/* Constructor.
 */
#define lru_new(p_this, print, x) \
  lru_newI((p_this), (void (*)(void *, Printer *))(print), (void *)(x))

void lru_newI(Lru **p_this,
    void (*print)(void *o, struct Printer *p),
    void *x);

/* Destructor.
 */
void lru_delete(Lru **p_this);

/* Print.
 */
void lru_print(Lru *this, struct Printer *p);

/* Add a reference.
 */
void lru_add(Lru *this, void *r);

/* Remove a reference.
 */
void lru_remove(Lru *this, void *r);

/* Touch a reference.
 */
void lru_touch(Lru *this, void *r);

/* Pop the least recently used reference.
 */
void *lru_pop(Lru *this);

/* Whether the set is empty.
 */
bool lru_is_empty(Lru *this);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
