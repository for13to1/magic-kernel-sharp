#include "Lru.h"

typedef struct {
  void *r;
  ListNode *node;
  Lru *this;
} Entry_;

static void entry_delete_(Entry_ **p_this);

static sbyte entry_compare_(Entry_ *a, Entry_ *b, void *x);

static void entry_print_(Entry_ *this, Printer *p);

static void list_print_(Dressed *d, Printer *p);


void lru_newI(Lru **p_this, void (*print)(void *o, Printer *p), void *x) {
  Lru *this;

  this = newo(p_this);

  this->print = print == null ? pointer_print : print;
  this->x = x;
  dynamic_set_new(
      (DynamicSet **)&this->_set,
      entry_delete_,
      entry_compare_,
      entry_print_);
  list_new((List **)&this->_list, delete_nothing, list_print_, x);
}

void lru_delete(Lru **p_this) {
  Lru *this;

  if ((this = *p_this) == null) {
    return;
  }

  list_delete((List **)&this->_list);
  dynamic_set_delete((DynamicSet **)&this->_set);

  deleteo(p_this);
}

void lru_print(Lru *this, Printer *p) {
  sprint(p, "Lru");
  list_print((List *)this->_list, p);
  fprint(p);
}

void lru_add(Lru *this, void *r) {
  Entry_ *entry;
  Entry_ *_entry = null;

  if (r == null) {
    throw("Reference cannot be null");
  }

  newo(&_entry);
  entry = _entry;
  _entry->r = r;
  _entry->this = this;
  _entry->node = null;

  if (dynamic_set_contains((DynamicSet *)this->_set, entry)) {
    throw("Trying to add reference %p to lru which is already added", r);
  }

  _entry->node = list_insert_head((List *)this->_list, &entry);
  dynamic_set_insert((DynamicSet *)this->_set, &_entry);
}

void lru_remove(Lru *this, void *r) {
  Entry_ key;
  Entry_ *_entry = null;

  key.r = r;
  dynamic_set_extract((DynamicSet *)this->_set, &key, &_entry);
  list_remove((List *)this->_list, _entry->node);
  entry_delete_(&_entry);
}

void lru_touch(Lru *this, void *r) {
  Entry_ key;
  Entry_ *entry;

  key.r = r;
  entry = dynamic_set_find((DynamicSet *)this->_set, &key);
  list_move_to_head((List *)this->_list, entry->node);
}

void *lru_pop(Lru *this) {
  void *r;
  Entry_ *entry = null;

  if (lru_is_empty(this)) {
    throw("Cannot pop from empty lru");
  }

  list_extract_tail((List *)this->_list, &entry);
  r = entry->r;
  dynamic_set_remove((DynamicSet *)this->_set, entry);

  return r;
}

bool lru_is_empty(Lru *this) {
  return dynamic_set_is_empty((DynamicSet *)this->_set);
}


static void entry_delete_(Entry_ **p_this) {
  deleteo(p_this);
}

static sbyte entry_compare_(Entry_ *a, Entry_ *b, void *x) {
  if (a->r < b->r) {
    return -1;
  }

  if (a->r > b->r) {
    return +1;
  }

  return 0;
}

static void entry_print_(Entry_ *this, Printer *p) {
  sprint(p, "Entry_");
  this->this->print(this->r, p);
  fprint(p);
}

static void list_print_(Dressed *d, Printer *p) {
  Entry_ *entry;

  entry = d->this;
  entry->this->print(entry->r, p);
}
