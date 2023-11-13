#include "List.h"

static void reset_nil_(List *this);

static ulong new_(ListNode **p_this, void **po, List *list);

static ulong delete_(ListNode **p_this, List *list);

static void print_(ListNode *this, Printer *p, List *list);

static ListNode *insert_(void *po, ListNode *after, List *list);

static void extract_(ListNode *node, void *po, List *list);

static void remove_(ListNode *node, List *list);

static void move_(ListNode *node, ListNode *after);


void list_newI(List **p_this,
    ulong (*destroy)(void **p_this),
    void (*print)(Dressed *d, Printer *p),
    void *x) {
  List *this;

  if (destroy == null) {
    throw("Destructor cannot be null");
  }

  this = newo(p_this);

  new_(&this->_nil, null, this);
  reset_nil_(this);

  this->destroy = destroy;
  this->print = print;
  this->x = x;
}

void list_delete(List **p_this) {
  ListNode *next;
  ListNode *node;
  List *this;

  if ((this = *p_this) == null) {
    return;
  }

  for (node = this->_nil->_next, next = node->_next;
      node != this->_nil;
      node = next, next = node->_next) {
    delete_(&node, this);
  }

  deleteo(&this->_nil);

  deleteo(p_this);
}

void list_print(List *this, Printer *p) {
  ListNode *node;

  sprint(p, "List");

  if ((node = this->_nil->_next) == this->_nil) {
    rprint(p, "(List is empty)");
  } else {
    for (; node != this->_nil; node = node->_next) {
      print_(node, p, this);
    }
  }

  fprint(p);
}

ListNode *list_insert_head(List *this, void *po) {
  return insert_(po, this->_nil, this);
}

ListNode *list_insert_tail(List *this, void *po) {
  return insert_(po, this->_nil->prev, this);
}

ListNode *list_insert_before(List *this, void *po, ListNode *node) {
  return insert_(po, node->prev, this);
}

ListNode *list_insert_after(List *this, void *po, ListNode *node) {
  return insert_(po, node, this);
}

void list_extract_head(List *this, void *po) {
  extract_(this->_nil->_next, po, this);
}

void list_remove_head(List *this) {
  remove_(this->_nil->_next, this);
}

void list_extract_tail(List *this, void *po) {
  extract_(this->_nil->prev, po, this);
}

void list_remove_tail(List *this) {
  remove_(this->_nil->prev, this);
}

void list_extract(List *this, void *po, ListNode *node) {
  extract_(node, po, this);
}

void list_remove(List *this, ListNode *node) {
  remove_(node, this);
}

void list_move_to_head(List *this, ListNode *node) {
  if (node->prev != this->_nil) {
    move_(node, this->_nil);
  }
}

void list_move_to_tail(List *this, ListNode *node) {
  if (node->_next != this->_nil) {
    move_(node, this->_nil->prev);
  }
}

bool list_is_empty(List *this) {
  return this->_nil->_next == this->_nil;
}

void list_verify_integrity(List *this) {
  ListNode *node;
  ListNode *prev;

  for (prev = this->_nil, node = prev->_next;
      ;
      prev = node, node = node->_next) {
    if (node->prev != prev) {
      throw("Impossible");
    }

    if (node == this->_nil) {
      break;
    }
  }
}


static void reset_nil_(List *this) {
  this->_nil->prev = this->_nil->_next = this->_nil;
}

static ulong new_(ListNode **p_this, void **po, List *list) {
  ListNode *this;

  this = newo(p_this);

  if (po != null) {
    donate(po, &this->_object);
  }

  this->prev = list->_nil;
  this->_next = list->_nil;

  return 0;
}

static ulong delete_(ListNode **p_this, List *list) {
  ListNode *this;

  if ((this = *p_this) == null || this == list->_nil) {
    return 0;
  }

  list->destroy(&this->_object);

  deleteo(p_this);

  return 0;
}

static void print_(ListNode *this, Printer *p, List *list) {
  Dressed d;

  if (this == list->_nil) {
    throw("Impossible: Trying to print nil node");
  }

  sprint(p, "ListNode");

  if (this->_object == null) {
    rprint(p, "Object is null!");
  } else if (list->print != null) {
    sprint(p, "Object");
    d.this = this->_object;
    d.x = list->x;
    list->print(&d, p);
    fprint(p);
  } else {
    rprint(p, "Object: %p", this->_object);
  }

  fprint(p);
}

static ListNode *insert_(void *po, ListNode *after, List *list) {
  ListNode *this = null;

  new_(&this, po, list);

  this->_next = after->_next;
  this->prev = after;
  after->_next->prev = this;
  after->_next = this;

  return this;
}

static void extract_(ListNode *node, void *po, List *list) {
  if (node == list->_nil) {
    throw("Cannot extract from empty list");
  }

  donate(&node->_object, po);
  remove_(node, list);
}

static void remove_(ListNode *node, List *list) {
  if (node == list->_nil) {
    throw("Cannot remove from empty list");
  }

  node->prev->_next = node->_next;
  node->_next->prev = node->prev;
  delete_(&node, list);
}

static void move_(ListNode *node, ListNode *after) {
  node->prev->_next = node->_next;
  node->_next->prev = node->prev;

  node->prev = after;
  node->_next = after->_next;
  after->_next->prev = node;
  after->_next = node;
}
