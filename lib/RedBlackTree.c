#include "RedBlackTree.h"

static void reset_nil_(RedBlackTree *this);

static ulong delete_(RedBlackNode **p_this, RedBlackTree *tree);

static ulong new_(RedBlackNode **p_this, void **po, RedBlackTree *tree);

static void print_(RedBlackNode *this, Printer *p, RedBlackTree *tree);

static void insert_fixup_(RedBlackTree *this, RedBlackNode *z);

static void remove_(RedBlackTree *this, RedBlackNode *z);

static RedBlackNode *find_(
    RedBlackNode *this,
    void *object,
    RedBlackTree *tree);

static RedBlackNode *minimum_(RedBlackNode *this, RedBlackTree *tree);

static RedBlackNode *maximum_(RedBlackNode *this, RedBlackTree *tree);

static void verify_integrity_(
    RedBlackNode *this,
    ulong *black_height,
    ulong current_black_height,
    RedBlackTree *tree);

static void left_rotate_(RedBlackTree *this, RedBlackNode *x);

static void right_rotate_(RedBlackTree *this, RedBlackNode *x);

static void transplant_(RedBlackTree *this, RedBlackNode *u, RedBlackNode *v);

static void delete_fixup_(RedBlackTree *this, RedBlackNode *x);

static void error_(RedBlackTree *this, RedBlackNode *node, char *message);

static void missing_(RedBlackTree *this, RedBlackNode *node);

static void repeated_(RedBlackTree *this, RedBlackNode *node);

#define blacken_(node, black, this) \
  blackenI_(node, black, this, __LINE__)

static void blackenI_(RedBlackNode *node,
    bool black,
    RedBlackTree *this,
    int line);


void red_black_tree_newI(
    RedBlackTree **p_this,
    ulong (*destroy)(void **p_this),
    sbyte (*compare)(void *a, void *b, void *x),
    void (*print)(Dressed *d, Printer *p),
    void *x) {
  RedBlackTree *this;

  if (destroy == null) {
    throw("Destructor cannot be null");
  }

  if (compare == null) {
    throw("Comparator cannot be null");
  }

  this = newo(p_this);

  new_(&this->_nil, null, this);
  reset_nil_(this);

  this->_root = this->_nil;
  this->destroy = destroy;
  this->compare = compare;
  this->print = print;
  this->x = x;
}

void red_black_tree_delete(RedBlackTree **p_this) {
  RedBlackTree *this;

  if ((this = *p_this) == null) {
    return;
  }

  this->_nil->_left = null;
  this->_nil->_right = null;

  delete_(&this->_root, this);
  deleteo(&this->_nil);

  deleteo(p_this);
}

void red_black_tree_print(RedBlackTree *this, Printer *p) {
  sprint(p, "RedBlackTree");

  if (this->_root == this->_nil) {
    rprint(p, "(No nodes)");
  } else {
    print_(this->_root, p, this);
  }

  fprint(p);
}

void red_black_tree_insert(RedBlackTree *this, void *po) {
  sbyte c;
  RedBlackNode *x;
  RedBlackNode *y;
  RedBlackNode *_z = null;

  new_(&_z, po, this);
  y = this->_nil;
  x = this->_root;

  while (x != this->_nil) {
    y = x;

    if ((c = this->compare(_z->_object, x->_object, this->x)) < 0) {
      x = x->_left;
    } else if (c > 0) {
      x = x->_right;
    } else {
      repeated_(this, _z);
    }
  }

  _z->parent = y;

  if (y == this->_nil) {
    this->_root = _z;
  } else if ((c = this->compare(_z->_object, y->_object, this->x)) < 0) {
    y->_left = _z;
  } else if (c > 0) {
    y->_right = _z;
  } else {
    repeated_(this, _z);
  }

  insert_fixup_(this, _z);
}

void red_black_tree_extract(RedBlackTree *this, void *key, void *po) {
  RedBlackNode *node;

  node = red_black_tree_find(this, key);

  if (node == this->_nil) {
    missing_(this, key);
  }

  donate(&node->_object, po);
  remove_(this, node);
}

void red_black_tree_remove(RedBlackTree *this, void *key) {
  RedBlackNode *node;

  node = red_black_tree_find(this, key);

  if (node == this->_nil) {
    missing_(this, key);
  }

  remove_(this, node);
}

RedBlackNode *red_black_tree_find(RedBlackTree *this, void *key) {
  return find_(this->_root, key, this);
}

bool red_black_tree_is_empty(RedBlackTree *this) {
  return this->_root == this->_nil;
}

RedBlackNode *red_black_tree_minimum(RedBlackTree *this) {
  return minimum_(this->_root, this);
}

RedBlackNode *red_black_tree_maximum(RedBlackTree *this) {
  return maximum_(this->_root, this);
}

RedBlackNode *red_black_tree_successor(RedBlackTree *this, RedBlackNode *node) {
  RedBlackNode *parent;

  if (node->_right != this->_nil) {
    return minimum_(node->_right, this);
  }

  for (parent = node->parent;
      parent != this->_nil && node == parent->_right;
      node = parent, parent = parent->parent) {
  }

  return parent;
}

RedBlackNode *red_black_tree_nil(RedBlackTree *this) {
  return this->_nil;
}


static void reset_nil_(RedBlackTree *this) {
  this->_nil->black = true;
  this->_nil->parent = this->_nil->_left = this->_nil->_right = this->_nil;
}

void red_black_tree_verify_integrity(RedBlackTree *this) {
  ulong black_height;
  RedBlackNode *root;

  if ((root = this->_root) == this->_nil) {
    return;
  }

  if (!root->black) {
    throw("Impossible: Root is red; should be black");
  }

  if (root->parent != this->_nil) {
    throw("Impossible: Root's parent is %p; should be nil == %p",
        root->parent,
        this->_nil);
  }

  black_height = 0;
  verify_integrity_(root, &black_height, 0, this);
}


static ulong delete_(RedBlackNode **p_this, RedBlackTree *tree) {
  RedBlackNode *this;

  if ((this = *p_this) == null || this == tree->_nil) {
    return 0;
  }

  delete_(&this->_left, tree);
  delete_(&this->_right, tree);
  tree->destroy(&this->_object);

  deleteo(p_this);

  return 0;
}

static ulong new_(RedBlackNode **p_this, void **po, RedBlackTree *tree) {
  RedBlackNode *this;

  this = newo(p_this);

  if (po != null) {
    donate(po, &this->_object);
  }

  this->_left = tree->_nil;
  this->_right = tree->_nil;

  return 0;
}

static void print_(RedBlackNode *this, Printer *p, RedBlackTree *tree) {
  Dressed d;

  if (this == tree->_nil) {
    throw("Impossible: Trying to print nil node");
  }

  sprint(p, "RedBlackNode");
  rprint(p, "Color: %s", this->black ? "black" : "red");

  if (this->_left != tree->_nil) {
    sprint(p, "Left");
    print_(this->_left, p, tree);
    fprint(p);
  }

  if (this->_object == null) {
    rprint(p, "Object is null!");
  } else if (tree->print != null) {
    sprint(p, "Object");
    d.this = this->_object;
    d.x = tree->x;
    tree->print(&d, p);
    fprint(p);
  } else {
    rprint(p, "Object: %p", this->_object);
  }

  if (this->_right != tree->_nil) {
    sprint(p, "Right");
    print_(this->_right, p, tree);
    fprint(p);
  }

  fprint(p);
}

static void insert_fixup_(RedBlackTree *this, RedBlackNode *z) {
  RedBlackNode *y;

  while (z != this->_nil && !z->parent->black) {
    if (z->parent == z->parent->parent->_left) {
      y = z->parent->parent->_right;

      if (!y->black) {
        blacken_(z->parent, true, this);
        blacken_(y, true, this);
        blacken_(z->parent->parent, false, this);
        z = z->parent->parent;
      } else {
        if (z == z->parent->_right) {
          z = z->parent;
          left_rotate_(this, z);
        }

        blacken_(z->parent, true, this);
        blacken_(z->parent->parent, false, this);
        right_rotate_(this, z->parent->parent);
      }
    } else {
      y = z->parent->parent->_left;

      if (!y->black) {
        blacken_(z->parent, true, this);
        blacken_(y, true, this);
        blacken_(z->parent->parent, false, this);
        z = z->parent->parent;
      } else {
        if (z == z->parent->_left) {
          z = z->parent;
          right_rotate_(this, z);
        }

        blacken_(z->parent, true, this);
        blacken_(z->parent->parent, false, this);
        left_rotate_(this, z->parent->parent);
      }
    }
  }

  blacken_(this->_root, true, this);
}

static void remove_(RedBlackTree *this, RedBlackNode *z) {
  bool y_orig_black;
  RedBlackNode *x;
  RedBlackNode *y;

  y = z;
  y_orig_black = y->black;

  if (z->_left == this->_nil) {
    x = z->_right;
    transplant_(this, z, z->_right);
  } else if (z->_right == this->_nil) {
    x = z->_left;
    transplant_(this, z, z->_left);
  } else {
    y = minimum_(z->_right, this);
    y_orig_black = y->black;
    x = y->_right;

    if (y->parent == z) {
      x->parent = y;
    } else {
      transplant_(this, y, y->_right);
      y->_right = z->_right;
      y->_right->parent = y;
    }

    transplant_(this, z, y);
    y->_left = z->_left;
    y->_left->parent = y;
    blacken_(y, z->black, this);
  }

  if (y_orig_black) {
    delete_fixup_(this, x);
  }

  z->_left = z->_right = this->_nil;
  delete_(&z, this);

  reset_nil_(this);
}

static RedBlackNode *find_(
    RedBlackNode *this,
    void *object,
    RedBlackTree *tree) {
  sbyte c;

  if (this == tree->_nil) {
    return tree->_nil;
  }

  c = tree->compare(object, this->_object, tree->x);

  if (c == 0) {
    return this;
  }

  if (c < 0) {
    return find_(this->_left, object, tree);
  }

  return find_(this->_right, object, tree);
}

static RedBlackNode *minimum_(RedBlackNode *this, RedBlackTree *tree) {
  if (this == tree->_nil) {
    return this;
  }

  while (this->_left != tree->_nil) {
    this = this->_left;
  }

  return this;
}

static RedBlackNode *maximum_(RedBlackNode *this, RedBlackTree *tree) {
  if (this == tree->_nil) {
    return this;
  }

  while (this->_right != tree->_nil) {
    this = this->_right;
  }

  return this;
}

static void verify_integrity_(
    RedBlackNode *this,
    ulong *black_height,
    ulong current_black_height,
    RedBlackTree *tree) {
  bool left_nil;
  bool right_nil;
  RedBlackNode *left;
  RedBlackNode *right;
  void *object;

  if (this == null) {
    throw("Impossible: Node is null; should be nil if empty");
  }

  if ((object = this->_object) == null) {
    throw("Impossible: Missing object");
  }

  if (object == tree->_nil) {
    throw("Impossible: Object set to nil; only nodes can be set to nil");
  }

  left = this->_left;
  right = this->_right;

  if (this->black) {
    current_black_height++;
  } else {
    if (!left->black) {
      throw("Impossible: Red node has red left child");
    }

    if (!right->black) {
      throw("Impossible: Red node has red right child");
    }
  }

  if (!(left_nil = left == tree->_nil)) {
    verify_integrity_(this->_left, black_height, current_black_height, tree);
  }

  if (!(right_nil = right == tree->_nil)) {
    verify_integrity_(this->_right, black_height, current_black_height, tree);
  }

  if (left_nil && right_nil) {
    if (*black_height == 0) {
      *black_height = current_black_height;
    } else {
      if (current_black_height != *black_height) {
        throw("Impossible: Black height %lu; previously found to be %lu",
            current_black_height,
            *black_height);
      }
    }
  }
}

static void left_rotate_(RedBlackTree *this, RedBlackNode *x) {
  RedBlackNode *y;

  y = x->_right;
  x->_right = y->_left;

  if (y->_left != this->_nil) {
    y->_left->parent = x;
  }

  y->parent = x->parent;

  if (x->parent == this->_nil) {
    this->_root = y;
  } else if (x == x->parent->_left) {
    x->parent->_left = y;
  } else {
    x->parent->_right = y;
  }

  y->_left = x;
  x->parent = y;
}

static void right_rotate_(RedBlackTree *this, RedBlackNode *y) {
  RedBlackNode *x;

  x = y->_left;
  y->_left = x->_right;

  if (x->_right != this->_nil) {
    x->_right->parent = y;
  }

  x->parent = y->parent;

  if (y->parent == this->_nil) {
    this->_root = x;
  } else if (y == y->parent->_right) {
    y->parent->_right = x;
  } else {
    y->parent->_left = x;
  }

  x->_right = y;
  y->parent = x;
}

static void transplant_(RedBlackTree *this, RedBlackNode *u, RedBlackNode *v) {
  if (u->parent == this->_nil) {
    this->_root = v;
  } else if (u == u->parent->_left) {
    u->parent->_left = v;
  } else {
    u->parent->_right = v;
  }

  v->parent = u->parent;
}

static void delete_fixup_(RedBlackTree *this, RedBlackNode *x) {
  RedBlackNode *w;

  while (x != this->_root && x->black) {
    if (x == x->parent->_left) {
      w = x->parent->_right;

      if (!w->black) {
        blacken_(w, true, this);
        blacken_(x->parent, false, this);
        left_rotate_(this, x->parent);
        w = x->parent->_right;
      }

      if (w->_left->black && w->_right->black) {
        blacken_(w, false, this);
        x = x->parent;
      } else {
        if (w->_right->black) {
          blacken_(w->_left, true, this);
          blacken_(w, false, this);
          right_rotate_(this, w);
          w = x->parent->_right;
        }

        blacken_(w, x->parent->black, this);
        blacken_(x->parent, true, this);
        blacken_(w->_right, true, this);
        left_rotate_(this, x->parent);
        x = this->_root;
      }
    } else {
      w = x->parent->_left;

      if (!w->black) {
        blacken_(w, true, this);
        blacken_(x->parent, false, this);
        right_rotate_(this, x->parent);
        w = x->parent->_left;
      }

      if (w->_right->black && w->_left->black) {
        blacken_(w, false, this);
        x = x->parent;
      } else {
        if (w->_left->black) {
          blacken_(w->_right, true, this);
          blacken_(w, false, this);
          left_rotate_(this, w);
          w = x->parent->_left;
        }

        blacken_(w, x->parent->black, this);
        blacken_(x->parent, true, this);
        blacken_(w->_left, true, this);
        right_rotate_(this, x->parent);
        x = this->_root;
      }
    }
  }

  blacken_(x, true, this);
}

static void error_(RedBlackTree *this, RedBlackNode *node, char *message) {
  char *_s = null;
  Dressed d;

  if (this->print != null) {
    d.this = node->_object;
    d.x = this->x;
    mprint(&_s, 1, this->print, &d);
  } else {
    string_new_f(&_s, "%p", node->_object);
  }

  throw("%s:\n%s", message, _s);
}

static void missing_(RedBlackTree *this, RedBlackNode *node) {
  error_(this, node, "Object is not in the tree");
}

static void repeated_(RedBlackTree *this, RedBlackNode *node) {
  error_(this, node, "Repeated object");
}

static void blackenI_(RedBlackNode *node,
    bool black,
    RedBlackTree *this,
    int line) {
  if (node == this->_nil && !black) {
    throw("Impossible: Line %d sets nil color to red!", line);
  }

  node->black = black;
}
