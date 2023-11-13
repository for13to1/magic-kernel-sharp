/* Object class implementing red-black trees, which take ownership of the
 * objects added to them.
 */
#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#ifdef __cplusplus
extern "C" {
#endif

struct RedBlackTree;

#include "Dressed.h"

typedef struct RedBlackNode {
  bool black;
  void *_object;
  struct RedBlackNode *_left;
  struct RedBlackNode *_right;
  struct RedBlackNode *parent;
} RedBlackNode;


typedef struct RedBlackTree {
  RedBlackNode *_nil;
  RedBlackNode *_root;
  void *x;
  ulong (*destroy)(void **p_this);
  sbyte (*compare)(void *a, void *b, void *x);
  void (*print)(struct Dressed *d, struct Printer *p);
} RedBlackTree;

/* Constructor.
 */
#define red_black_tree_new(p_this, destroy, compare, print, x) \
  red_black_tree_newI( \
      (p_this), \
      (ulong (*)(void **))(destroy), \
      (sbyte (*)(void *, void *, void *))(compare), \
      (print), \
      (void *)(x))

void red_black_tree_newI(
    RedBlackTree **p_this,
    ulong (*destroy)(void **p_this),
    sbyte (*compare)(void *a, void *b, void *x),
    void (*print)(struct Dressed *d, struct Printer *p),
    void *x);

/* Destructor.
 */
void red_black_tree_delete(RedBlackTree **p_this);

/* Print.
 */
void red_black_tree_print(RedBlackTree *this, struct Printer *p);

/* Insert an object into the tree, which takes ownership of the object.
 */
void red_black_tree_insert(RedBlackTree *this, void *po);

/* Extract an object from the tree, which finds the object in the tree that
 * compares equal to the key, returns ownership of that found object to the
 * calling code, and removes that node of the tree.
 */
void red_black_tree_extract(RedBlackTree *this, void *key, void *po);

/* Remove an object from the tree, which finds the object in the tree that
 * compares equal to the key, destroys that object, and removes that node from
 * the tree.
 */
void red_black_tree_remove(RedBlackTree *this, void *key);

/* Find the node with the object that compares equal to the given key.
 */
RedBlackNode *red_black_tree_find(RedBlackTree *this, void *key);

/* Whether the tree is empty.
 */
bool red_black_tree_is_empty(RedBlackTree *this);

/* Get the minimum node in the tree.
 */
RedBlackNode *red_black_tree_minimum(RedBlackTree *this);

/* Get the maximum node in the tree.
 */
RedBlackNode *red_black_tree_maximum(RedBlackTree *this);

/* Get the successor node in the tree.
 */
RedBlackNode *red_black_tree_successor(RedBlackTree *this, RedBlackNode *node);

/* Return the nil node for this tree.
 */
RedBlackNode *red_black_tree_nil(RedBlackTree *this);

/* Verify integrity of the tree.
 */
void red_black_tree_verify_integrity(RedBlackTree *this);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
