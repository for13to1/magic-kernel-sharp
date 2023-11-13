#include "DynamicSet.h"

static void print_(Dressed *d, Printer *p);


void dynamic_set_newI(
    DynamicSet **p_this,
    ulong (*destroy)(void **p_this),
    sbyte (*compare)(void *a, void *b, void *x),
    void (*print)(void *this, Printer *p)) {
  DynamicSet *this;

  this = newo(p_this);

  this->print = print;

  red_black_tree_new(
      &this->_tree,
      destroy,
      compare,
      print_,
      this);
}

void dynamic_set_delete(DynamicSet **p_this) {
  DynamicSet *this;

  if ((this = *p_this) == null) {
    return;
  }

  red_black_tree_delete(&this->_tree);

  deleteo(p_this);
}

void dynamic_set_print(DynamicSet *this, Printer *p) {
  sprint(p, "DynamicSet");
  red_black_tree_print(this->_tree, p);
  fprint(p);
}

void dynamic_set_insert(DynamicSet *this, void *po) {
  red_black_tree_insert(this->_tree, po);
}

void dynamic_set_extract(DynamicSet *this, void *key, void *po) {
  red_black_tree_extract(this->_tree, key, po);
}

void dynamic_set_remove(DynamicSet *this, void *key) {
  red_black_tree_remove(this->_tree, key);
}

void *dynamic_set_find(DynamicSet *this, void *key) {
  RedBlackNode *node;

  node = red_black_tree_find(this->_tree, key);

  if (node == red_black_tree_nil(this->_tree)) {
    return null;
  }

  return node->_object;
}

bool dynamic_set_is_empty(DynamicSet *this) {
  return red_black_tree_is_empty(this->_tree);
}

bool dynamic_set_contains(DynamicSet *this, void *key) {
  return dynamic_set_find(this, key) != null;
}

void dynamic_set_foreachI(
    DynamicSet *this,
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
  DynamicSet *this;

  this = d->x;

  sprint(p, "DynamicObject");

  if (this->print == null || d->this == null) {
    rprint(p, "%p", d->this);
  } else {
    this->print(d->this, p);
  }

  fprint(p);
}
