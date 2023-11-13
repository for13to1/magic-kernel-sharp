/* Object class that allows a set of objects to be built and searched while it
 * is being built (hence, "dynamic"). Thin wrapper around a RedBlackTree.
 */
#ifndef DYNAMIC_SET_H
#define DYNAMIC_SET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Printer.h"
#include "RedBlackTree.h"

typedef struct {
  struct RedBlackTree *_tree;
  void (*print)(void *this, struct Printer *p);
} DynamicSet;

/* Constructor. If the compare() and print() methods are null, it is assumed
 * that the pointers themselves are to be compared.
 */
#define dynamic_set_new(p_this, destroy, compare, print) \
  dynamic_set_newI( \
      (p_this), \
      (ulong (*)(void **))(destroy), \
      (sbyte (*)(void *, void *, void *))(compare), \
      (void (*)(void *, Printer *))(print))

void dynamic_set_newI(
    DynamicSet **p_this,
    ulong (*destroy)(void **p_this),
    sbyte (*compare)(void *a, void *b, void *x),
    void (*print)(void *this, struct Printer *p));

/* Destructor.
 */
void dynamic_set_delete(DynamicSet **p_this);

/* Print.
 */
void dynamic_set_print(DynamicSet *this, struct Printer *p);

/* Insert an object into the set, which takes ownership. It must not compare
 * equal to any object in the set.
 */
void dynamic_set_insert(DynamicSet *this, void *p_o);

/* Extract the object in the set that compares equal to the given key, giving
 * ownership of the object to the calling code, and removing it from the set.
 */
void dynamic_set_extract(DynamicSet *this, void *key, void *p_o);

/* Remove the object in the set that compares equal to the given key,
 * destroying the object and removing it from the set.
 */
void dynamic_set_remove(DynamicSet *this, void *key);

/* Find the object in the set that compares equal to the given key, or null if
 * is not in the set.
 */
void *dynamic_set_find(DynamicSet *this, void *key);

/* Whether the set is empty.
 */
bool dynamic_set_is_empty(DynamicSet *this);

/* Whether the set contains any object that compares equal to the given key.
 */
bool dynamic_set_contains(DynamicSet *this, void *ikey);

/* Walk through the set and call the given callback method on each object.
 */
#define dynamic_set_foreach(this, callback, x) \
  dynamic_set_foreachI((this), (void (*)(void *, void *))(callback), (void *)x);

void dynamic_set_foreachI(
    DynamicSet *this,
    void (*callback)(void *this, void *x),
    void *x);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
