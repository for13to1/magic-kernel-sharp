#include "RedBlackTreeTest.h"

typedef struct {
  short s;
} Short_;

typedef struct {
  int i;
} Int_;

static void short_new_(Short_ **p_this, short s);

static void short_delete_(Short_ **p_this);

static void short_print_(Dressed *d, Printer *p);

static void short_insert_(RedBlackTree *tree, short value, bool verify);

static short short_extract_(RedBlackTree *tree, short key, bool verify);

static void short_remove_(RedBlackTree *tree, short key, bool verify);

static void int_new_(Int_ **p_this, int i);

static void int_delete_(Int_ **p_this);

static void int_print_(Dressed *d, Printer *p);

static void int_insert_(RedBlackTree *tree, int value, bool verify);

static int int_extract_(RedBlackTree *tree, int key, bool verify);

static void int_remove_(RedBlackTree *tree, int key, bool verify);

static void test_(ulong n,
    int *values,
    ulong check_index_1,
    Int_ *check_1,
    ulong check_index_2,
    Int_ *check_2,
    bool verify);


START

TEST(RedBlackTree, Simple) {
  RedBlackNode *n9;
  RedBlackNode *n24;
  RedBlackNode *n66;
  RedBlackNode *nil;
  Short_ *_s = null;
  RedBlackTree *_tree = null;

  red_black_tree_new(
      &_tree,
      short_delete_,
      short_compare,
      short_print_,
      null);

  /* Empty tree.
   */
  EXPECT_TRUE(red_black_tree_is_empty(_tree));
  nil = red_black_tree_nil(_tree);
  short_new_(&_s, 9);
  EXPECT_PEQ(nil, red_black_tree_find(_tree, _s));
  EXPECT_PEQ(nil, red_black_tree_minimum(_tree));
  EXPECT_PEQ(nil, red_black_tree_successor(_tree, nil));

  /* Insert one value.
   */
  short_insert_(_tree, 24, true);
  EXPECT_FALSE(red_black_tree_is_empty(_tree));
  _s->s = 24;
  n24 = red_black_tree_find(_tree, _s);
  EXPECT_ULEQ(24, ((Short_ *)n24->_object)->s);
  EXPECT_PEQ(n24, red_black_tree_minimum(_tree));
  EXPECT_PEQ(n24, red_black_tree_maximum(_tree));
  EXPECT_PEQ(nil, red_black_tree_successor(_tree, n24));

  /* Insert another, smaller value.
   */
  short_insert_(_tree, 9, true);
  _s->s = 9;
  n9 = red_black_tree_find(_tree, _s);
  EXPECT_ULEQ(9, ((Short_ *)n9->_object)->s);
  EXPECT_PEQ(n9, red_black_tree_minimum(_tree));
  EXPECT_PEQ(n24, red_black_tree_maximum(_tree));
  EXPECT_PEQ(n24, red_black_tree_successor(_tree, n9));
  EXPECT_PEQ(nil, red_black_tree_successor(_tree, n24));

  /* Insert another, larger value.
   */
  short_insert_(_tree, 66, true);
  _s->s = 66;
  n66 = red_black_tree_find(_tree, _s);
  EXPECT_ULEQ(66, ((Short_ *)n66->_object)->s);
  EXPECT_PEQ(n9, red_black_tree_minimum(_tree));
  EXPECT_PEQ(n66, red_black_tree_maximum(_tree));
  EXPECT_PEQ(n24, red_black_tree_successor(_tree, n9));
  EXPECT_PEQ(n66, red_black_tree_successor(_tree, n24));
  EXPECT_PEQ(nil, red_black_tree_successor(_tree, n66));

  /* Extract the middle value.
   */
  EXPECT_LEQ(24, short_extract_(_tree, 24, true));

  /* Extract the smaller value.
   */
  EXPECT_LEQ(9, short_extract_(_tree, 9, true));

  /* Extract the larger value.
   */
  EXPECT_LEQ(66, short_extract_(_tree, 66, true));
  EXPECT_TRUE(red_black_tree_is_empty(_tree));

  /* Insert the values again.
   */
  short_insert_(_tree, 66, true);
  short_insert_(_tree, 24, true);
  short_insert_(_tree, 9, true);

  /* Remove the middle one.
   */
  short_remove_(_tree, 24, true);

  /* Remove the larger one.
   */
  short_remove_(_tree, 66, true);

  /* Remove the smaller one.
   */
  short_remove_(_tree, 9, true);
  EXPECT_TRUE(red_black_tree_is_empty(_tree));

  red_black_tree_delete(&_tree);
  short_delete_(&_s);
}

TEST(RedBlackTree, Print) {
  char *_s = null;
  RedBlackTree *_ = null;

  red_black_tree_new(&_, int_delete_, int_compare, int_print_, null);
  aprint(&_s, 0, red_black_tree, _);
  EXPECT_STREQ("[RedBlackTree]\n. (No nodes)\n[/RedBlackTree]\n", _s);
  string_delete(&_s);

  int_insert_(_, 24, true);
  aprint(&_s, 0, red_black_tree, _);
  EXPECT_STREQ(
      "[RedBlackTree]\n. [RedBlackNode]\n. . Color: black\n. . [Object]\n"
      ". . . [Int_]\n. . . . Value: 24\n"
      ". . . [/Int_]\n. . [/Object]\n. [/RedBlackNode]\n[/RedBlackTree]\n",
      _s);
  string_delete(&_s);

  red_black_tree_delete(&_);
}

TEST(RedBlackTree, Ascending) {
  ulong n;
  ulong u;
  int *_values = null;
  Int_ check1 = {-5};
  Int_ check2 = {4};
  Int_ check3 = {2521};
  Int_ check4 = {2425};
  Int_ check5 = {-113392};
  Int_ check6 = {335980};

  /* Small run of ascending values.
   */
  n = 10;
  new(&_values, n);

  for (u = 0; u < n; u++) {
    _values[u] = u - 5;
  }

  CALL(test_(n, _values, 0, &check1, 9, &check2, true));
  deletev(&_values);

  /* Larger run of ascending values.
   */
  n = 1000;
  new(&_values, n);

  for (u = 0; u < n; u++) {
    _values[u] = 3 * u + 28;
  }

  CALL(test_(n, _values, 831, &check3, 799, &check4, true));
  deletev(&_values);

  /* Even larger run of ascending values. Don't check integrity of entire tree
   * on each insert.
   */
  n = 100000;
  new(&_values, n);

  for (u = 0; u < n; u++) {
    _values[u] = 7 * (u - 40000) + 2409;
  }

  CALL(test_(n, _values, 23457, &check5, 87653, &check6, false));
  deletev(&_values);
}

TEST(RedBlackTree, Descending) {
  ulong n = 10;
  ulong u;
  int *_values = null;
  Int_ check1 = {-1};
  Int_ check2 = {2};
  Int_ check3 = {-476};
  Int_ check4 = {-572};
  Int_ check5 = {-2811402};
  Int_ check6 = {2131844};

  /* Small run of descending values.
   */
  new(&_values, n);

  for (u = 0; u < n; u++) {
    _values[u] = 5 - u;
  }

  test_(n, _values, 3, &check1, 6, &check2, true);
  deletev(&_values);

  /* Larger run of ascending values.
   */
  n = 1000;
  new(&_values, n);

  for (u = 0; u < n; u++) {
    _values[u] = -3 * u + 28;
  }

  test_(n, _values, 831, &check3, 799, &check4, true);
  deletev(&_values);

  /* Even larger run of ascending values. Don't check integrity of entire tree
   * on each insert.
   */
  n = 100000;
  new(&_values, n);

  for (u = 0; u < n; u++) {
    _values[u] = -77 * (u - 40000) + 2409;
  }

  test_(n, _values, 23456, &check5, 87654, &check6, false);
  deletev(&_values);
}

TEST(RedBlackTree, Random) {
  int *_values = null;
  ulong n;
  ulong u;

  /* Small run of random values.
   */
  n = 10;
  new(&_values, n);

  for (u = 0; u < n; u++) {
    _values[u] = 5 - u;
  }

  shuffle(_values, n);
  test_(n, _values, 0, null, 0, null, true);
  deletev(&_values);

  /* Larger run of ascending values.
   */
  n = 1000;
  new(&_values, n);

  for (u = 0; u < n; u++) {
    _values[u] = -3 * u + 28;
  }

  shuffle(_values, n);
  test_(n, _values, 0, null, 0, null, true);
  deletev(&_values);

  /* Even larger run of ascending values. Don't check integrity of entire tree
   * on each insert.
   */
  n = 100000;
  new(&_values, n);

  for (u = 0; u < n; u++) {
    _values[u] = -77 * (u - 40000) + 2409;
  }

  shuffle(_values, n);
  test_(n, _values, 0, null, 0, null, false);
  deletev(&_values);
}

FINISH


static void short_new_(Short_ **p_this, short s) {
  Short_ *this;

  newo(p_this);
  this = *p_this;

  this->s = s;
}

static void short_delete_(Short_ **p_this) {
  Short_ *this;

  if ((this = *p_this) == null) {
    return;
  }

  deleteo(p_this);
}

static void short_print_(Dressed *d, Printer *p) {
  Short_ *this;

  this = d->this;

  sprint(p, "Short_");
  rprint(p, "Value: %d", this->s);
  fprint(p);
}

static void short_insert_(RedBlackTree *tree, short value, bool verify) {
  Short_ *_s = null;

  if (verify) {
    red_black_tree_verify_integrity(tree);
  }

  short_new_(&_s, value);
  red_black_tree_insert(tree, &_s);

  if (verify) {
    red_black_tree_verify_integrity(tree);
  }
}

static short short_extract_(RedBlackTree *tree, short key, bool verify) {
  short r;
  Short_ s;
  Short_ *_s = null;

  if (verify) {
    red_black_tree_verify_integrity(tree);
  }

  s.s = key;
  red_black_tree_extract(tree, &key, &_s);
  r = _s->s;
  short_delete_(&_s);

  if (verify) {
    red_black_tree_verify_integrity(tree);
  }

  return r;
}

static void short_remove_(RedBlackTree *tree, short key, bool verify) {
  Short_ s;

  if (verify) {
    red_black_tree_verify_integrity(tree);
  }

  s.s = key;
  red_black_tree_remove(tree, &key);

  if (verify) {
    red_black_tree_verify_integrity(tree);
  }
}

static void int_new_(Int_ **p_this, int i) {
  Int_ *this;

  newo(p_this);
  this = *p_this;

  this->i = i;
}

static void int_delete_(Int_ **p_this) {
  Int_ *this;

  if ((this = *p_this) == null) {
    return;
  }

  deleteo(p_this);
}

static int int_extract_(RedBlackTree *tree, int key, bool verify) {
  int r;
  Int_ i;
  Int_ *_i = null;

  if (verify) {
    red_black_tree_verify_integrity(tree);
  }

  i.i = key;
  red_black_tree_extract(tree, &key, &_i);
  r = _i->i;
  int_delete_(&_i);

  if (verify) {
    red_black_tree_verify_integrity(tree);
  }

  return r;
}

static void int_remove_(RedBlackTree *tree, int key, bool verify) {
  Int_ i;

  if (verify) {
    red_black_tree_verify_integrity(tree);
  }

  i.i = key;
  red_black_tree_remove(tree, &key);

  if (verify) {
    red_black_tree_verify_integrity(tree);
  }
}

static void int_print_(Dressed *d, Printer *p) {
  Int_ *this;

  this = d->this;

  sprint(p, "Int_");
  rprint(p, "Value: %d", this->i);
  fprint(p);
}

static void int_insert_(RedBlackTree *tree, int value, bool verify_integrity) {
  Int_ *_i = null;

  if (verify_integrity) {
    red_black_tree_verify_integrity(tree);
  }

  int_new_(&_i, value);
  red_black_tree_insert(tree, (void **)&_i);

  if (verify_integrity) {
    red_black_tree_verify_integrity(tree);
  }
}

static void test_(ulong n,
    int *values,
    ulong check_index_1,
    Int_ *check_1,
    ulong check_index_2,
    Int_ *check_2,
    bool verify_integrity) {
  Int_ i;
  ulong u;
  void **_objects = null;
  RedBlackNode *node;
  RedBlackNode *found;
  RedBlackTree *_tree = null;
  RandomCoin *_coin = null;

  red_black_tree_new(&_tree, int_delete_, int_compare, int_print_, null);
  random_coin_new(&_coin);

  for (u = 0; u < n; u++) {
    int_insert_(_tree, values[u], verify_integrity);
  }

  red_black_tree_verify_integrity(_tree);
  qsortx(values, n, int_compare, false, null);

  i.i = values[0];
  node = red_black_tree_minimum(_tree);

  for (u = 0; u < n; u++) {
    i.i = values[u];
    found = red_black_tree_find(_tree, &i);
    EXPECT_PEQ(found, node);
    EXPECT_ULEQ(values[u], ((Int_ *)node->_object)->i);
    node = red_black_tree_successor(_tree, node);
  }

  EXPECT_PEQ(red_black_tree_nil(_tree), node);
  deletev(&_objects);

  shuffle(values, n);

  for (u = 0; u < n; u++) {
    if (random_coin_flip(_coin)) {
      EXPECT_LEQ(values[u], int_extract_(_tree, values[u], verify_integrity));
    } else {
      int_remove_(_tree, values[u], verify_integrity);
    }
  }

  EXPECT_TRUE(red_black_tree_is_empty(_tree));

  random_coin_delete(&_coin);
  red_black_tree_delete(&_tree);
}
