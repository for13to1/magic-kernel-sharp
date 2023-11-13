/* Object class implementing doubly-linked lists, which takes ownership of the
 * objects added to them.
 */
#ifndef LIST_H
#define LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Dressed.h"

typedef struct ListNode {
  void *_object;
  struct ListNode *prev;
  struct ListNode *_next;
} ListNode;

typedef struct {
  ListNode *_nil;
  void *x;
  ulong (*destroy)(void **p_this);
  void (*print)(struct Dressed *d, struct Printer *p);
} List;

/* Constructor.
 */
#define list_new(p_this, destroy, print, x) \
  list_newI( \
      (p_this), \
      (ulong (*)(void **))(destroy), \
      (print), \
      (void *)(x))

void list_newI(List **p_this,
    ulong (*destroy)(void **p_this),
    void (*print)(struct Dressed *d, struct Printer *p),
    void *x);

/* Destructor.
 */
void list_delete(List **p_this);

/* Print.
 */
void list_print(List *this, struct Printer *p);

/* Insert an object into the head of the list.
 */
ListNode *list_insert_head(List *this, void *po);

/* Insert an object into the tail of the list.
 */
ListNode *list_insert_tail(List *this, void *po);

/* Insert an object into the list, before a given node.
 */
ListNode *list_insert_before(List *this, void *po, ListNode *node);

/* Insert an object into the list, after a given node.
 */
ListNode *list_insert_after(List *this, void *po, ListNode *node);

/* Extract the object at the head of the list.
 */
void list_extract_head(List *this, void *po);

/* Remove the object at the head of the list.
 */
void list_remove_head(List *this);

/* Extract the object at the tail of the list.
 */
void list_extract_tail(List *this, void *po);

/* Remove the object at the tail of the list.
 */
void list_remove_tail(List *this);

/* Extract the object at a given node in the list.
 */
void list_extract(List *this, void *po, ListNode *node);

/* Remove the object at a given node in the list.
 */
void list_remove(List *this, ListNode *node);

/* Move a given node to the head of the list.
 */
void list_move_to_head(List *this, ListNode *node);

/* Move a given node to the tail of the list.
 */
void list_move_to_tail(List *this, ListNode *node);

/* Whether the list is empty.
 */
bool list_is_empty(List *this);

/* Verify the integrity of the list.
 */
void list_verify_integrity(List *this);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
