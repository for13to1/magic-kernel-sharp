/* Object class that allows a set of reference pointers to built and searched
 * while it is being built (hence, "dynamic"). It uses a RedBlackTree with an
 * object destructor that does nothing.
 */
#ifndef DYNAMIC_REFERENCE_SET_H
#define DYNAMIC_REFERENCE_SET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "RedBlackTree.h"

typedef struct {
  struct RedBlackTree *_tree;
  void (*print)(void *this, struct Printer *p);
} DynamicReferenceSet;

/* Constructor.
 */
#define dynamic_reference_set_new(p_this) \
  dynamic_reference_set_new_specify(p_this, null, null);

/* Constructor, specifying compare() and print() methods; if they are null, the
 * raw pointer versions are used.
 */
#define dynamic_reference_set_new_specify(p_this, compare, print) \
  dynamic_reference_set_new_specifyI( \
      (p_this), \
      (sbyte (*)(void *, void *, void *))(compare), \
      (void (*)(void *, Printer *))(print))

void dynamic_reference_set_new_specifyI(
    DynamicReferenceSet **p_this,
    sbyte (*compare)(void *a, void *b, void *x),
    void (*print)(void *this, struct Printer *p));

/* Destructor.
 */
void dynamic_reference_set_delete(DynamicReferenceSet **p_this);

/* Print.
 */
void dynamic_reference_set_print(DynamicReferenceSet *this, struct Printer *p);

/* Add a reference.
 */
void dynamic_reference_set_add(DynamicReferenceSet *this, void *r);

/* Remove a reference.
 */
void dynamic_reference_set_remove(DynamicReferenceSet *this, void *r);

/* Find the object in the set that compares equal to the given reference.
 */
void *dynamic_reference_set_find(DynamicReferenceSet *this, void *r);

/* Whether the set contains a given reference.
 */
bool dynamic_reference_set_contains(DynamicReferenceSet *this, void *r);

/* Whether the set is empty.
 */
bool dynamic_reference_set_is_empty(DynamicReferenceSet *this);

/* Walk through the set and call the given callback method on each object.
 */
#define dynamic_reference_set_foreach(this, callback, x) \
  dynamic_reference_set_foreachI( \
      (this), \
      (void (*)(void *, void *))(callback), \
      (void *)x);

void dynamic_reference_set_foreachI(
    DynamicReferenceSet *this,
    void (*callback)(void *this, void *x),
    void *x);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
