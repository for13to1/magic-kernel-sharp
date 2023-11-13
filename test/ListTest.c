#include "ListTest.h"

static void short_new_(short **p_this, short s);

static void short_delete_(short **p_this);

static void short_print_(Dressed *d, Printer *p);


START

TEST(List, Simple) {
  short *_s = null;
  ListNode *node;
  ListNode *n123;
  ListNode *n2001;
  ListNode *n24;
  ListNode *n456;
  List *_list = null;

  list_new(&_list, short_delete_, short_print_, null);
  EXPECT_TRUE(list_is_empty(_list));
  list_verify_integrity(_list);

  short_new_(&_s, 24);
  list_insert_head(_list, &_s);
  EXPECT_FALSE(list_is_empty(_list));
  list_verify_integrity(_list);
  node = _list->_nil->_next;
  EXPECT_LEQ(24, *(short *)node->_object);
  node = node->_next;
  EXPECT_PEQ(node, _list->_nil);

  short_new_(&_s, -9);
  list_insert_head(_list, &_s);
  list_verify_integrity(_list);
  node = _list->_nil->_next;
  EXPECT_LEQ(-9, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(24, *(short *)node->_object);
  node = node->_next;
  EXPECT_PEQ(node, _list->_nil);

  short_new_(&_s, 1966);
  list_insert_head(_list, &_s);
  list_verify_integrity(_list);
  node = _list->_nil->_next;
  EXPECT_LEQ(1966, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-9, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(24, *(short *)node->_object);
  node = node->_next;
  EXPECT_PEQ(node, _list->_nil);
  node = _list->_nil->prev;
  EXPECT_LEQ(24, *(short *)node->_object);
  node = node->prev;
  EXPECT_LEQ(-9, *(short *)node->_object);
  node = node->prev;
  EXPECT_LEQ(1966, *(short *)node->_object);
  node = node->prev;
  EXPECT_PEQ(node, _list->_nil);

  short_new_(&_s, -7);
  list_insert_tail(_list, &_s);
  list_verify_integrity(_list);
  node = _list->_nil->_next;
  EXPECT_LEQ(1966, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-9, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(24, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-7, *(short *)node->_object);
  node = node->_next;
  EXPECT_PEQ(node, _list->_nil);

  short_new_(&_s, -7);
  list_insert_tail(_list, &_s);
  list_verify_integrity(_list);
  node = _list->_nil->_next;
  EXPECT_LEQ(1966, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-9, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(24, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-7, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-7, *(short *)node->_object);
  node = node->_next;
  EXPECT_PEQ(node, _list->_nil);

  short_new_(&_s, 1968);
  list_insert_tail(_list, &_s);
  list_verify_integrity(_list);
  node = _list->_nil->_next;
  EXPECT_LEQ(1966, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-9, *(short *)node->_object);
  node = node->_next;
  n24 = node;
  EXPECT_LEQ(24, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-7, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-7, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(1968, *(short *)node->_object);
  node = node->_next;
  EXPECT_PEQ(node, _list->_nil);

  short_new_(&_s, 42);
  list_insert_before(_list, &_s, n24);
  list_verify_integrity(_list);
  node = _list->_nil->_next;
  EXPECT_LEQ(1966, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-9, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(42, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(24, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-7, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-7, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(1968, *(short *)node->_object);
  node = node->_next;
  EXPECT_PEQ(node, _list->_nil);

  short_new_(&_s, 2001);
  list_insert_after(_list, &_s, n24);
  list_verify_integrity(_list);
  node = _list->_nil->_next;
  EXPECT_LEQ(1966, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-9, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(42, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(24, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(2001, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-7, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-7, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(1968, *(short *)node->_object);
  node = node->_next;
  EXPECT_PEQ(node, _list->_nil);

  list_extract_head(_list, &_s);
  list_verify_integrity(_list);
  EXPECT_LEQ(1966, *_s);
  short_delete_(&_s);
  node = _list->_nil->_next;
  EXPECT_LEQ(-9, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(42, *(short *)node->_object);

  list_remove_head(_list);
  list_verify_integrity(_list);
  node = _list->_nil->_next;
  EXPECT_LEQ(42, *(short *)node->_object);

  list_extract_tail(_list, &_s);
  list_verify_integrity(_list);
  EXPECT_LEQ(1968, *_s);
  short_delete_(&_s);
  node = _list->_nil->prev;
  EXPECT_LEQ(-7, *(short *)node->_object);
  node = node->prev;
  EXPECT_LEQ(-7, *(short *)node->_object);

  list_remove_tail(_list);
  list_verify_integrity(_list);
  node = _list->_nil->prev;
  EXPECT_LEQ(-7, *(short *)node->_object);

  list_extract(_list, &_s, n24);
  list_verify_integrity(_list);
  EXPECT_LEQ(24, *_s);
  short_delete_(&_s);
  node = _list->_nil->_next;
  EXPECT_LEQ(42, *(short *)node->_object);
  node = node->_next;
  n2001 = node;
  EXPECT_LEQ(2001, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-7, *(short *)node->_object);
  node = node->_next;
  EXPECT_PEQ(node, _list->_nil);

  list_remove(_list, n2001);
  list_verify_integrity(_list);
  node = _list->_nil->_next;
  EXPECT_LEQ(42, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-7, *(short *)node->_object);
  node = node->_next;
  EXPECT_PEQ(node, _list->_nil);

  short_new_(&_s, 123);
  n123 = list_insert_tail(_list, &_s);
  short_new_(&_s, 456);
  n456 = list_insert_tail(_list, &_s);
  short_new_(&_s, -789);
  list_insert_tail(_list, &_s);

  list_move_to_head(_list, n123);
  node = _list->_nil->_next;
  EXPECT_LEQ(123, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(42, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-7, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(456, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-789, *(short *)node->_object);
  node = node->_next;
  EXPECT_PEQ(node, _list->_nil);

  list_move_to_tail(_list, n456);
  node = _list->_nil->_next;
  EXPECT_LEQ(123, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(42, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-7, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(-789, *(short *)node->_object);
  node = node->_next;
  EXPECT_LEQ(456, *(short *)node->_object);
  node = node->_next;
  EXPECT_PEQ(node, _list->_nil);

  list_delete(&_list);
}

FINISH


static void short_new_(short **p_this, short s) {
  short *this;

  newo(p_this);
  this = *p_this;

  *this = s;
}

static void short_delete_(short **p_this) {
  short *this;

  if ((this = *p_this) == null) {
    return;
  }

  deleteo(p_this);
}

static void short_print_(Dressed *d, Printer *p) {
  short *this;

  this = d->this;

  sprint(p, "short");
  rprint(p, "Value: %d", *this);
  fprint(p);
}
