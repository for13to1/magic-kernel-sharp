#include "DynamicReferenceSet.h"

static void print_(Dressed *d, Printer *p);


void dynamic_reference_set_new_specifyI(
    DynamicReferenceSet **p_this,
    sbyte (*compare)(void *a, void *b, void *x),
    void (*print)(void *this, Printer *p)) {
  DynamicReferenceSet *this;

  this = newo(p_this);

  this->print = print;

  red_black_tree_new(
      &this->_tree,
      delete_nothing,
      compare == null ? pointer_compare : compare,
      print_,
      this);
}

void dynamic_reference_set_delete(DynamicReferenceSet **p_this) {
  DynamicReferenceSet *this;

  if ((this = *p_this) == null) {
    return;
  }

  red_black_tree_delete(&this->_tree);

  deleteo(p_this);
}

void dynamic_reference_set_print(DynamicReferenceSet *this, Printer *p) {
  sprint(p, "DynamicReferenceSet");
  red_black_tree_print(this->_tree, p);
  fprint(p);
}

void dynamic_reference_set_add(DynamicReferenceSet *this, void *r) {
  if (dynamic_reference_set_contains(this, r)) {
    return;
  }

  red_black_tree_insert(this->_tree, &r);
}

void dynamic_reference_set_remove(DynamicReferenceSet *this, void *r) {
  if (!dynamic_reference_set_contains(this, r)) {
    throw("Reference is not in the set");
  }

  red_black_tree_remove(this->_tree, r);
}

void *dynamic_reference_set_find(DynamicReferenceSet *this, void *r) {
  RedBlackNode *node;

  node = red_black_tree_find(this->_tree, r);

  return node == red_black_tree_nil(this->_tree) ? null : node->_object;
}

bool dynamic_reference_set_contains(DynamicReferenceSet *this, void *r) {
  return red_black_tree_find(this->_tree, r) != red_black_tree_nil(this->_tree);
}

bool dynamic_reference_set_is_empty(DynamicReferenceSet *this) {
  return red_black_tree_is_empty(this->_tree);
}

void dynamic_reference_set_foreachI(
    DynamicReferenceSet *this,
    void (*callback)(void *this, void *x),
    void *x) {
  RedBlackNode *last_node;
  RedBlackNode *node;

  if (red_black_tree_is_empty(this->_tree)) {
    return;
  }

  node = red_black_tree_minimum(this->_tree);
  last_node = red_black_tree_maximum(this->_tree);

  for (;;) {
    callback(node->_object, x);

    if (node == last_node) {
      break;
    }

    node = red_black_tree_successor(this->_tree, node);
  }
}

static void print_(Dressed *d, Printer *p) {
  DynamicReferenceSet *this;

  this = d->x;

  sprint(p, "DynamicReference");

  if (this->print == null) {
    rprint(p, "%p", d->this);
  } else {
    this->print(d->this, p);
  }

  fprint(p);
}
