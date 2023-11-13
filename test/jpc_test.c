#include "jpc_test.h"

#define N_ 100000

typedef struct {
  bool b: 1;
} Boolbit_;

START

TEST(Type, Size) {
  ASSERT_ULEQ(1, sizeof(sbyte));
  ASSERT_ULEQ(1, sizeof(byte));

  ASSERT_ULEQ(2, sizeof(short));
  ASSERT_ULEQ(2, sizeof(ushort));

  ASSERT_ULEQ(4, sizeof(int));
  ASSERT_ULEQ(4, sizeof(uint));

  ASSERT_ULEQ(8, sizeof(long));
  ASSERT_ULEQ(8, sizeof(ulong));

  ASSERT_ULEQ(4, sizeof(float));
  ASSERT_ULEQ(8, sizeof(double));

  ASSERT_ULEQ(8, sizeof(size_t));
  ASSERT_ULEQ(8, sizeof(byte *));
  ASSERT_ULEQ(8, sizeof(ulong *));
  ASSERT_ULEQ(8, sizeof(void (*)(void)));
}

TEST(Integers, TwosComplement) {
  union {
    ulong u;
    long l;
  } c;

  c.l = -1;
  ASSERT_ULEQ(ULONG_MAX, c.u);
}

TEST(Null, Value) {
  ASSERT_PEQ(0, null);
}

TEST(Boolean, Size) {
  ASSERT_ULEQ(1, sizeof(bool));
}

TEST(Boolean, Values) {
  ASSERT_ULEQ(0, false);
  ASSERT_ULEQ(!0, true);
}

TEST(Boolbit, Basic) {
  bool d[32];
  byte a;
  byte v;
  ulong u;
  struct {
    boolbit a: 1;
    boolbit b: 1;
    boolbit c: 1;
    boolbit d: 1;
    boolbit e: 1;
    boolbit f: 1;
    boolbit g: 1;
    boolbit h: 1;
    boolbit i: 1;
    boolbit j: 1;
    boolbit k: 1;
    boolbit l: 1;
    boolbit m: 1;
    boolbit n: 1;
    boolbit o: 1;
    boolbit p: 1;
    boolbit q: 1;
    boolbit r: 1;
    boolbit s: 1;
    boolbit t: 1;
    boolbit u: 1;
    boolbit v: 1;
    boolbit w: 1;
    boolbit x: 1;
    boolbit y: 1;
    boolbit z: 1;
    boolbit A: 1;
    boolbit B: 1;
    boolbit C: 1;
    boolbit D: 1;
    boolbit E: 1;
    boolbit F: 1;
  } s;
  Random *_random = null;

  ASSERT_ULEQ(4, sizeof(s));
  random_new(&_random);

  for (u = 0; u < N_; u++) {
    for (v = 0; v < 32; v++) {
      d[v] = random_bool(_random);
    }

    a = 0;
    s.a = d[a++];
    s.b = d[a++];
    s.c = d[a++];
    s.d = d[a++];
    s.e = d[a++];
    s.f = d[a++];
    s.g = d[a++];
    s.h = d[a++];
    s.i = d[a++];
    s.j = d[a++];
    s.k = d[a++];
    s.l = d[a++];
    s.m = d[a++];
    s.n = d[a++];
    s.o = d[a++];
    s.p = d[a++];
    s.q = d[a++];
    s.r = d[a++];
    s.s = d[a++];
    s.t = d[a++];
    s.u = d[a++];
    s.v = d[a++];
    s.w = d[a++];
    s.x = d[a++];
    s.y = d[a++];
    s.z = d[a++];
    s.A = d[a++];
    s.B = d[a++];
    s.C = d[a++];
    s.D = d[a++];
    s.E = d[a++];
    s.F = d[a++];
    a = 0;
    ASSERT_ULEQ(d[a], s.a);
    a++;
    ASSERT_ULEQ(d[a], s.b);
    a++;
    ASSERT_ULEQ(d[a], s.c);
    a++;
    ASSERT_ULEQ(d[a], s.d);
    a++;
    ASSERT_ULEQ(d[a], s.e);
    a++;
    ASSERT_ULEQ(d[a], s.f);
    a++;
    ASSERT_ULEQ(d[a], s.g);
    a++;
    ASSERT_ULEQ(d[a], s.h);
    a++;
    ASSERT_ULEQ(d[a], s.i);
    a++;
    ASSERT_ULEQ(d[a], s.j);
    a++;
    ASSERT_ULEQ(d[a], s.k);
    a++;
    ASSERT_ULEQ(d[a], s.l);
    a++;
    ASSERT_ULEQ(d[a], s.m);
    a++;
    ASSERT_ULEQ(d[a], s.n);
    a++;
    ASSERT_ULEQ(d[a], s.o);
    a++;
    ASSERT_ULEQ(d[a], s.p);
    a++;
    ASSERT_ULEQ(d[a], s.q);
    a++;
    ASSERT_ULEQ(d[a], s.r);
    a++;
    ASSERT_ULEQ(d[a], s.s);
    a++;
    ASSERT_ULEQ(d[a], s.t);
    a++;
    ASSERT_ULEQ(d[a], s.u);
    a++;
    ASSERT_ULEQ(d[a], s.v);
    a++;
    ASSERT_ULEQ(d[a], s.w);
    a++;
    ASSERT_ULEQ(d[a], s.x);
    a++;
    ASSERT_ULEQ(d[a], s.y);
    a++;
    ASSERT_ULEQ(d[a], s.z);
    a++;
    ASSERT_ULEQ(d[a], s.A);
    a++;
    ASSERT_ULEQ(d[a], s.B);
    a++;
    ASSERT_ULEQ(d[a], s.C);
    a++;
    ASSERT_ULEQ(d[a], s.D);
    a++;
    ASSERT_ULEQ(d[a], s.E);
    a++;
    ASSERT_ULEQ(d[a], s.F);
  }
  random_delete(&_random);
}

TEST(Boolean, Identities) {
  bool b;
  Boolbit_ bb;

  b = true;
  bb.b = true;
  ASSERT_TRUE(b);
  ASSERT_TRUE((1 == 1) == b);
  ASSERT_TRUE(bb.b);
  ASSERT_TRUE((1 == 1) == bb.b);
  ASSERT_TRUE(bb.b == b);
  ASSERT_FALSE(bb.b != b);

  b = true;
  bb.b = false;
  ASSERT_TRUE(b);
  ASSERT_FALSE(bb.b);
  ASSERT_FALSE(bb.b == b);
  ASSERT_TRUE(bb.b != b);

  b = false;
  bb.b = true;
  ASSERT_FALSE(b);
  ASSERT_TRUE(bb.b);
  ASSERT_FALSE(bb.b == b);
  ASSERT_TRUE(bb.b != b);

  b = false;
  bb.b = false;
  ASSERT_FALSE(b);
  ASSERT_TRUE((1 == 0) == b);
  ASSERT_FALSE(bb.b);
  ASSERT_TRUE((1 == 0) == bb.b);
  ASSERT_TRUE(bb.b == b);
  ASSERT_FALSE(bb.b != b);
}

TEST(Memory, Instance) {
  double *_d = null;
  double *d2;

  newo(&_d);
  d2 = _d;

  /* Should be allocated, and set in both pointers.
   */
  EXPECT_NONNULL(_d);
  EXPECT_PEQ(d2, _d);

  /* Delete should set the pointer to be empty.
   */
  deleteo(&_d);
  EXPECT_NULL(_d);

  /* Reference will not be empty. Do not use!
   */
  EXPECT_NONNULL(d2);

  /* Second deleteo should do nothing.
   */
  deleteo(&_d);
}

TEST(Memory, ValueArray) {
  int *_a = null;
  ulong n;
  ulong u;

  /* Test new(), with three elements of 4-byte ints.
   */
  new(&_a, 3);

  /* Memory should all be zeroed on allocation.
   */
  for (u = 0; u < 3; u++) {
    EXPECT_ULEQ(0, _a[u]);
  }

  /* Set some nonzero values.
   */
  _a[0] = 24;
  _a[1] = 9;
  _a[2] = 1966;

  /* Test renew(), with an extra four elements of 4-byte ints.
   */
  renew(&_a, 3, 7, null);

  /* Should not affect existing entries.
   */
  EXPECT_ULEQ(24, _a[0]);
  EXPECT_ULEQ(9, _a[1]);
  EXPECT_ULEQ(1966, _a[2]);

  /* New elements should be zeroed.
   */
  for (u = 3; u < 7; u++) {
    EXPECT_ULEQ(0, _a[u]);
  }

  /* Test renew() to a smaller size, where five elements of 4-byte ints are
   * freed.
   */
  renew(&_a, 7, 2, null);

  /* Should not affect surviving entries.
   */
  EXPECT_ULEQ(24, _a[0]);
  EXPECT_ULEQ(9, _a[1]);

  /* Test renewup(), with one extra 4-byte int element.
   */
  renewup(&_a, 2, 3);

  /* New element should be zeroed.
   */
  EXPECT_ULEQ(0, _a[2]);

  /* Test renewup1().
   */
  renewup1(&_a, 3);

  /* New element should be zeroed.
   */
  EXPECT_ULEQ(0, _a[3]);

  /* Store the length of our array in a variable.
   */
  n = 4;

  /* Test append_copy().
   */
  append_copy(&_a, n, -24091966);

  /* Length should have been incremented.
   */
  EXPECT_ULEQ(5, n);

  /* Expect the value to now be at the end of the array.
   */
  EXPECT_LEQ(-24091966, _a[n - 1]);

  /* Test deletev(), where three elements of 4-byte ints are freed.
   */
  deletev(&_a);

  /* Second deletev() should be no-op, with no change in memory.
   */
  deletev(&_a);

  /* n = 0 is ok, and allocates one byte (not one element).
   */
  new(&_a, 0);

  /* Renewing up to five elements of 4-byte ints is 20 bytes, minus the one
   * that we allocated originally.
   */
  renewv(&_a, 0, 5);

  /* Renewing down to an empty array frees the five 4-byte ints, but allocates
   * a single byte.
   */
  renewv(&_a, 5, 0);

  /* Deleting the array itself frees that byte.
   */
  deletev(&_a);
}

TEST(Memory, ObjectArray) {
  char *_s = null;
  char **_a = null;
  ulong n;
  ulong u;

  /* Test new(), creating three 8-byte char pointers.
   */
  new(&_a, 3);

  /* Memory should all be zeroed on allocation.
   */
  for (u = 0; u < 3; u++) {
    EXPECT_NULL(_a[u]);
  }

  /* Create some string objects of different sizes.
   */
  string_new_copy(&_a[0], "dog");
  string_new_copy(&_a[1], "woof");
  string_new_copy(&_a[2], "miaow");

  /* Check that they are there.
   */
  EXPECT_STREQ("dog", _a[0]);
  EXPECT_STREQ("woof", _a[1]);
  EXPECT_STREQ("miaow", _a[2]);

  /* Test renew() up, where we create four more 8-byte char pointers.
   */
  renew(&_a, 3, 7, string_delete);

  /* Should not affect existing entries.
   */
  EXPECT_STREQ("dog", _a[0]);
  EXPECT_STREQ("woof", _a[1]);
  EXPECT_STREQ("miaow", _a[2]);

  /* New elements should be null.
   */
  for (u = 3; u < 7; u++) {
    EXPECT_NULL(_a[u]);
  }

  /* Test renew() to a smaller size, where we free five 8-byte char pointers,
   * as well as the the 6 bytes for _a[2].
   */
  renew(&_a, 7, 2, string_delete);

  /* Should not affect surviving entries.
   */
  EXPECT_STREQ("dog", _a[0]);
  EXPECT_STREQ("woof", _a[1]);

  /* Test renewup().
   */
  renewup(&_a, 2, 3);

  /* New element should be zeroed.
   */
  EXPECT_NULL(_a[2]);

  /* Test renewup1().
   */
  renewup1(&_a, 3);

  /* New element should be zeroed.
   */
  EXPECT_NULL(_a[3]);

  /* Put the length of array into a variable.
   */
  n = 4;

  /* Create a string.
   */
  string_new_copy(&_s, "cat");

  /* Donate-append it to the array.
   */
  append_donate(&_a, n, &_s);

  /* Length should have been incremented.
   */
  EXPECT_ULEQ(5, n);

  /* The string pointer should now be empty.
   */
  EXPECT_NULL(_s);

  /* Expect the string to now be owned by the array.
   */
  EXPECT_STREQ("cat", _a[n - 1]);

  /* Test delete().
   */
  delete(&_a, n, string_delete);

  /* Second delete() should be no-op.
   */
  delete(&_a, n, string_delete);

  /* n = 0 is ok, and allocates one byte (not one element).
   */
  new(&_a, 0);

  /* Renew up to five elements of 8-byte char pointers, minus the original
   * byte.
   */
  renew(&_a, 0, 5, string_delete);

  /* Create an object.
   */
  string_new_copy(&_a[2], "!");

  /* Renewing back down to zero elements frees those five 8-byte pointers, as
   * well as the two bytes of the object created, but leaves a single byte.
   */
  renew(&_a, 5, 0, string_delete);

  /* Deleting the array frees that single byte.
   */
  delete(&_a, 0, string_delete);
}

TEST(Memory, ValueMatrix) {
  int i;
  int j;
  uint **_a = null;
  short **_b = null;

  /* A value matrix has the memory of the elements plus a pointer for each row,
   * so if the size of each element is s and the dimensions are m x n, the
   * memory should be m * (8 + n * s), since pointers are 8-byte.
   */
  newm(&_a, 2, 3);

  /* All elements should be zeroed on allocation.
   */
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      EXPECT_ULEQ(0, _a[i][j]);
    }
  }

  /* Set some elements.
   */
  _a[0][0] = 24;
  _a[0][1] = 9;
  _a[0][2] = 1966;
  _a[1][0] = 7;
  _a[1][1] = 7;
  _a[1][2] = 1968;

  /* Check that they are correctly stored.
   */
  EXPECT_ULEQ(24, _a[0][0]);
  EXPECT_ULEQ(9, _a[0][1]);
  EXPECT_ULEQ(1966, _a[0][2]);
  EXPECT_ULEQ(7, _a[1][0]);
  EXPECT_ULEQ(7, _a[1][1]);
  EXPECT_ULEQ(1968, _a[1][2]);

  /* Destructor should return the same memory.
   */
  deletemv(&_a, 2);

  /* _b is of type short which is 2 bytes per element.
   */
  newm(&_b, 2, 3);

  /* Should be zeroed on allocation.
   */
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      EXPECT_DEQ(0, _b[i][j]);
    }
  }

  /* Set some elements.
   */
  _b[0][0] = 24;
  _b[0][1] = -9;
  _b[0][2] = 1966;
  _b[1][0] = 7;
  _b[1][1] = -7;
  _b[1][2] = 1968;

  /* Check that they are correctly stored.
   */
  EXPECT_LEQ(24, _b[0][0]);
  EXPECT_LEQ(-9, _b[0][1]);
  EXPECT_LEQ(1966, _b[0][2]);
  EXPECT_LEQ(7, _b[1][0]);
  EXPECT_LEQ(-7, _b[1][1]);
  EXPECT_LEQ(1968, _b[1][2]);

  /* Destructor should return the same memory.
   */
  deletemv(&_b, 2);
}

TEST(Memory, ObjectMatrix) {
  int i;
  int j;
  char ***_a = null;

  /* An object matrix has the same memory as a value matrix, where s = 8 as it
   * contains pointers, plus the memory of any created elements.
   */
  newm(&_a, 2, 3);

  /* All elements should be null.
   */
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      EXPECT_NULL(_a[i][j]);
    }
  }

  /* Set some elements.
   */
  string_new_copy(&_a[0][0], "dog");
  string_new_copy(&_a[0][2], "woof");
  string_new_copy(&_a[1][2], "miaow");

  /* Check that they are correctly stored.
   */
  EXPECT_STREQ("dog", _a[0][0]);
  EXPECT_STREQ("woof", _a[0][2]);
  EXPECT_STREQ("miaow", _a[1][2]);

  /* Destructor should return the same memory as the constructor, plus all the
   * memory of the created objects.
   */
  deletem(&_a, 2, 3, string_delete);
}

TEST(Memory, ValueRaggedMatrix) {
  int i;
  int j;
  uint **_a = null;
  short **_b = null;
  ulong dims[] = {3, 2, 1};

  /* A value matrix has the memory of the elements plus a pointer for each row.
   * so if the size of each element is s and the dimensions are n0 x n1s, the
   * memory should be \sum_i0 (8 + n1s[i0] * s), since pointers are 8-byte.
   */
  newrm(&_a, 3, dims);

  /* All elements should be zeroed on allocation.
   */
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3 - i; j++) {
      EXPECT_ULEQ(0, _a[i][j]);
    }
  }

  /* Set all elements.
   */
  _a[0][0] = 24;
  _a[0][1] = 9;
  _a[0][2] = 1966;
  _a[1][0] = 7;
  _a[1][1] = 7;
  _a[2][0] = 1968;

  /* Check that they are correctly stored.
   */
  EXPECT_ULEQ(24, _a[0][0]);
  EXPECT_ULEQ(9, _a[0][1]);
  EXPECT_ULEQ(1966, _a[0][2]);
  EXPECT_ULEQ(7, _a[1][0]);
  EXPECT_ULEQ(7, _a[1][1]);
  EXPECT_ULEQ(1968, _a[2][0]);

  /* Destructor should return the same memory.
   */
  deletermv(&_a, 3);

  /* _b is of type short which is 2 bytes per element.
   */
  newrm(&_b, 3, dims);

  /* All elements should be zeroed on allocation.
   */
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3 - i; j++) {
      EXPECT_ULEQ(0, _b[i][j]);
    }
  }

  /* Set all elements.
   */
  _b[0][0] = 24;
  _b[0][1] = 9;
  _b[0][2] = 1966;
  _b[1][0] = 7;
  _b[1][1] = 7;
  _b[2][0] = 1968;

  /* Check that they are correctly stored.
   */
  EXPECT_ULEQ(24, _b[0][0]);
  EXPECT_ULEQ(9, _b[0][1]);
  EXPECT_ULEQ(1966, _b[0][2]);
  EXPECT_ULEQ(7, _b[1][0]);
  EXPECT_ULEQ(7, _b[1][1]);
  EXPECT_ULEQ(1968, _b[2][0]);

  /* Destructor should return the same memory.
   */
  deletermv(&_b, 3);
}

TEST(Memory, ObjectRaggedMatrix) {
  int i;
  int j;
  char ***_a = null;
  ulong dims[] = {3, 2, 1};

  /* An object matrix has the same memory as a value matrix, where s = 8 as it
   * contains pointers, plus the memory of any created elements.
   */
  newrm(&_a, 3, dims);

  /* All elements should be null.
   */
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3 - i; j++) {
      EXPECT_NULL(_a[i][j]);
    }
  }

  /* Set some elements.
   */
  string_new_copy(&_a[0][2], "dog");
  string_new_copy(&_a[1][1], "woof");
  string_new_copy(&_a[2][0], "miaow");

  /* Check that they are correctly stored.
   */
  EXPECT_STREQ("dog", _a[0][2]);
  EXPECT_STREQ("woof", _a[1][1]);
  EXPECT_STREQ("miaow", _a[2][0]);

  /* Destructor should return the same memory as the constructor, plus all the
   * memory of the created objects.
   */
  deleterm(&_a, 3, dims, string_delete);
}

TEST(Memory, ValueTensor3) {
  int i;
  int j;
  int k;
  int ***_a = null;

  /* A 3-dimensional tensor has a pointer and a matrix for each value of the
   * first index, so with dimensions n0 x n1 x n2 and the size of each element
   * s the memory should be n0 * (8 + n1 * (8 + n2 * s)).
   */
  newt3(&_a, 2, 3, 4);

  /* All elements should be zeroed on allocation.
   */
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      for (k = 0; k < 4; k++) {
        EXPECT_ULEQ(0, _a[i][j][k]);
      }
    }
  }

  /* Set some values.
   */
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      for (k = 0; k < 4; k++) {
        _a[i][j][k] = -2 * i + 3 + j + 7 * k;
      }
    }
  }

  /* Check those values.
   */
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      for (k = 0; k < 4; k++) {
        EXPECT_ULEQ(_a[i][j][k], -2 * i + 3 + j + 7 * k);
      }
    }
  }

  /* Destructor should return the same memory.
   */
  deletet3v(&_a, 2, 3);
}

TEST(Memory, ObjectTensor3) {
  int i;
  int j;
  int k;
  char ****_a = null;

  /* An object matrix has the same memory as a value matrix, where s = 8 as it
   * contains pointers, plus the memory of any created elements.
   */
  newt3(&_a, 2, 3, 4);

  /* All elements should be null.
   */
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      for (k = 0; k < 4; k++) {
        EXPECT_NULL(_a[i][j][k]);
      }
    }
  }

  /* Set some elements.
   */
  string_new_copy(&_a[0][0][2], "dog");
  string_new_copy(&_a[0][2][1], "woof");
  string_new_copy(&_a[1][2][3], "miaow");

  /* Check that they are correctly stored.
   */
  EXPECT_STREQ("dog", _a[0][0][2]);
  EXPECT_STREQ("woof", _a[0][2][1]);
  EXPECT_STREQ("miaow", _a[1][2][3]);

  /* Destructor should return the same memory as the constructor, plus all the
   * memory of the created objects.
   */
  deletet3(&_a, 2, 3, 4, string_delete);
}

TEST(Memory, ValueTensor4) {
  int i;
  int j;
  int k;
  int l;
  int ****_a = null;

  /* A 4-dimensional tensor has a pointer and a 3-d tensor for each value of
   * the first index, so with dimensions n0 x n1 x n2 and the size of each
   * element s the memory should be n0 * (8 + n1 * (8 + n2 * (8 + n3 * s))).
   */
  newt4(&_a, 2, 3, 4, 7);

  /* All elements should be zeroed on allocation.
   */
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      for (k = 0; k < 4; k++) {
        for (l = 0; l < 7; l++) {
          EXPECT_ULEQ(0, _a[i][j][k][l]);
        }
      }
    }
  }

  /* Set some values.
   */
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      for (k = 0; k < 4; k++) {
        for (l = 0; l < 7; l++) {
          _a[i][j][k][l] = -2 * i + 3 + j + 7 * k - 3 * l * i;
        }
      }
    }
  }

  /* Check those values.
   */
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      for (k = 0; k < 4; k++) {
        for (l = 0; l < 7; l++) {
          EXPECT_ULEQ(_a[i][j][k][l], -2 * i + 3 + j + 7 * k - 3 * l * i);
        }
      }
    }
  }

  /* Destructor should return the same memory.
   */
  deletet4v(&_a, 2, 3, 4);
}

TEST(Memory, ObjectTensor4) {
  int i;
  int j;
  int k;
  int l;
  char *****_a = null;

  /* An object matrix has the same memory as a value matrix, where s = 8 as it
   * contains pointers, plus the memory of any created elements.
   */
  newt4(&_a, 2, 3, 4, 7);

  /* All elements should be null.
   */
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      for (k = 0; k < 4; k++) {
        for (l = 0; l < 7; l++) {
          EXPECT_NULL(_a[i][j][k][l]);
        }
      }
    }
  }

  /* Set some elements.
   */
  string_new_copy(&_a[0][0][2][1], "dog");
  string_new_copy(&_a[0][2][1][0], "woof");
  string_new_copy(&_a[1][2][3][6], "miaow");

  /* Check that they are correctly stored.
   */
  EXPECT_STREQ("dog", _a[0][0][2][1]);
  EXPECT_STREQ("woof", _a[0][2][1][0]);
  EXPECT_STREQ("miaow", _a[1][2][3][6]);

  /* Destructor should return the same memory as the constructor, plus all the
   * memory of the created objects.
   */
  deletet4(&_a, 2, 3, 4, 7, string_delete);
}

TEST(Memory, Donate) {
  double *_src = null;
  double *_dst = null;
  double *ref;

  newo(&_src);
  ref = _src;
  donate(&_src, &_dst);
  EXPECT_NULL(_src);
  EXPECT_PEQ(_dst, ref);
  deleteo(&_dst);
}

TEST(Math, Round) {
  EXPECT_DEQ(0, round(0));
  EXPECT_DEQ(0, round(0.49999999));
  EXPECT_DEQ(1, round(0.5));
  EXPECT_DEQ(2, round(1.5));
  EXPECT_DEQ(100, round(99.5));
  EXPECT_DEQ(100, round(100.4999));
  EXPECT_DEQ(0, round(-0.49999999));
  EXPECT_DEQ(-1, round(-0.5));
  EXPECT_DEQ(-1, round(-0.9));
  EXPECT_DEQ(-1, round(-1.49999999));
  EXPECT_DEQ(-2, round(-1.5));
}

TEST(Boolean, String) {
  EXPECT_STREQ("true", bool_string(true));
  EXPECT_STREQ("false", bool_string(false));
}

TEST(String, Plural) {
  EXPECT_STREQ("b", plural(-12345, "a", "b"));
  EXPECT_LEQ(2, plural(-1, 1, 2));
  EXPECT_STREQ("cd", plural(0, "ab", "cd"));
  EXPECT_STREQ("ab", plural(1, "ab", "cd"));
  EXPECT_STREQ("s", plural(2, "", "s"));
  EXPECT_LEQ(-1, plural(12345, 1, -1));
}

TEST(String, PluralS) {
  EXPECT_STREQ("s", plural_s(-12345));
  EXPECT_STREQ("s", plural_s(-1));
  EXPECT_STREQ("s", plural_s(0));
  EXPECT_STREQ("", plural_s(1));
  EXPECT_STREQ("s", plural_s(2));
  EXPECT_STREQ("s", plural_s(12345));
}

TEST(Math, Maximum) {
  EXPECT_LEQ(6, maximum(-1, 6));
  EXPECT_LEQ(6, maximum(6, -1));
  EXPECT_LEQ(1, maximum(1, -6));
  EXPECT_LEQ(1, maximum(-6, 1));
  EXPECT_LEQ(6, maximum(6, 6));
  EXPECT_LEQ(-1, maximum(-1, -1));
  EXPECT_LEQ(0, maximum(0, 0));
  EXPECT_LEQ(0, maximum(0, -4));
  EXPECT_LEQ(0, maximum(-4, 0));
  EXPECT_LEQ(4, maximum(0, 4));
  EXPECT_LEQ(4, maximum(4, 0));
}

TEST(Math, Minimum) {
  EXPECT_LEQ(-1, minimum(-1, 6));
  EXPECT_LEQ(-1, minimum(6, -1));
  EXPECT_LEQ(-6, minimum(1, -6));
  EXPECT_LEQ(-6, minimum(-6, 1));
  EXPECT_LEQ(6, minimum(6, 6));
  EXPECT_LEQ(-1, minimum(-1, -1));
  EXPECT_LEQ(0, minimum(0, 0));
  EXPECT_LEQ(-4, minimum(0, -4));
  EXPECT_LEQ(-4, minimum(-4, 0));
  EXPECT_LEQ(0, minimum(0, 4));
  EXPECT_LEQ(0, minimum(4, 0));
}

TEST(Math, LowerBound) {
  EXPECT_DEQ(1.2, lower_bound(-4, 1.2));
  EXPECT_DEQ(1.2, lower_bound(1.2, 1.2));
  EXPECT_DEQ(1.3, lower_bound(1.3, 1.2));
}

TEST(Math, UpperBound) {
  EXPECT_DEQ(-4.2, upper_bound(-4.2, 4));
  EXPECT_DEQ(4, upper_bound(4, 4));
  EXPECT_DEQ(4, upper_bound(4.3, 4));
}

TEST(Math, Bound) {
  EXPECT_DEQ(1.2, bound(-4.2, 1.2, 4));
  EXPECT_DEQ(1.2, bound(1.2, 1.2, 4));
  EXPECT_DEQ(1.3, bound(1.3, 1.2, 4));
  EXPECT_DEQ(4, bound(4, 1.2, 4));
  EXPECT_DEQ(4, bound(4.1, 1.2, 4));
}

TEST(Integer, Modulo) {
  EXPECT_LEQ(0, modulo(-3, 1));
  EXPECT_LEQ(0, modulo(-2, 1));
  EXPECT_LEQ(0, modulo(-1, 1));
  EXPECT_LEQ(0, modulo(0, 1));
  EXPECT_LEQ(0, modulo(1, 1));
  EXPECT_LEQ(0, modulo(2, 1));
  EXPECT_LEQ(0, modulo(3, 1));

  EXPECT_LEQ(0, modulo(-6, 2));
  EXPECT_LEQ(1, modulo(-5, 2));
  EXPECT_LEQ(0, modulo(-4, 2));
  EXPECT_LEQ(1, modulo(-3, 2));
  EXPECT_LEQ(0, modulo(-2, 2));
  EXPECT_LEQ(1, modulo(-1, 2));
  EXPECT_LEQ(0, modulo(0, 2));
  EXPECT_LEQ(1, modulo(1, 2));
  EXPECT_LEQ(0, modulo(2, 2));
  EXPECT_LEQ(1, modulo(3, 2));
  EXPECT_LEQ(0, modulo(4, 2));
  EXPECT_LEQ(1, modulo(5, 2));

  EXPECT_LEQ(0, modulo(-9, 3));
  EXPECT_LEQ(1, modulo(-8, 3));
  EXPECT_LEQ(2, modulo(-7, 3));
  EXPECT_LEQ(0, modulo(-6, 3));
  EXPECT_LEQ(1, modulo(-5, 3));
  EXPECT_LEQ(2, modulo(-4, 3));
  EXPECT_LEQ(0, modulo(-3, 3));
  EXPECT_LEQ(1, modulo(-2, 3));
  EXPECT_LEQ(2, modulo(-1, 3));
  EXPECT_LEQ(0, modulo(0, 3));
  EXPECT_LEQ(1, modulo(1, 3));
  EXPECT_LEQ(2, modulo(2, 3));
  EXPECT_LEQ(0, modulo(3, 3));
  EXPECT_LEQ(1, modulo(4, 3));
  EXPECT_LEQ(2, modulo(5, 3));
  EXPECT_LEQ(0, modulo(6, 3));
  EXPECT_LEQ(1, modulo(7, 3));
  EXPECT_LEQ(2, modulo(8, 3));

  /* Test different types. Type coercion can yield wrong results if the macro
   * is not set up correctly.
   */
  EXPECT_LEQ(9, modulo(-1, 10));
  EXPECT_LEQ(9, modulo((long)-1, 10));
  EXPECT_LEQ(9, modulo(-1, (ulong)10));
  EXPECT_LEQ(9, modulo((long)-1, (ulong)10));
}

TEST(Math, IntegerDivision) {
  EXPECT_LEQ(-3, intdiv(-3, 1));
  EXPECT_LEQ(-2, intdiv(-2, 1));
  EXPECT_LEQ(-1, intdiv(-1, 1));
  EXPECT_LEQ(0, intdiv(0, 1));
  EXPECT_LEQ(1, intdiv(1, 1));
  EXPECT_LEQ(2, intdiv(2, 1));
  EXPECT_LEQ(3, intdiv(3, 1));

  EXPECT_LEQ(-3, intdiv(-6, 2));
  EXPECT_LEQ(-3, intdiv(-5, 2));
  EXPECT_LEQ(-2, intdiv(-4, 2));
  EXPECT_LEQ(-2, intdiv(-3, 2));
  EXPECT_LEQ(-1, intdiv(-2, 2));
  EXPECT_LEQ(-1, intdiv(-1, 2));
  EXPECT_LEQ(0, intdiv(0, 2));
  EXPECT_LEQ(0, intdiv(1, 2));
  EXPECT_LEQ(1, intdiv(2, 2));
  EXPECT_LEQ(1, intdiv(3, 2));
  EXPECT_LEQ(2, intdiv(4, 2));
  EXPECT_LEQ(2, intdiv(5, 2));
  EXPECT_LEQ(3, intdiv(6, 2));

  EXPECT_LEQ(-3, intdiv(-9, 3));
  EXPECT_LEQ(-3, intdiv(-8, 3));
  EXPECT_LEQ(-3, intdiv(-7, 3));
  EXPECT_LEQ(-2, intdiv(-6, 3));
  EXPECT_LEQ(-2, intdiv(-5, 3));
  EXPECT_LEQ(-2, intdiv(-4, 3));
  EXPECT_LEQ(-1, intdiv(-3, 3));
  EXPECT_LEQ(-1, intdiv(-2, 3));
  EXPECT_LEQ(-1, intdiv(-1, 3));
  EXPECT_LEQ(0, intdiv(0, 3));
  EXPECT_LEQ(0, intdiv(1, 3));
  EXPECT_LEQ(0, intdiv(2, 3));
  EXPECT_LEQ(1, intdiv(3, 3));
  EXPECT_LEQ(1, intdiv(4, 3));
  EXPECT_LEQ(1, intdiv(5, 3));
  EXPECT_LEQ(2, intdiv(6, 3));
  EXPECT_LEQ(2, intdiv(7, 3));
  EXPECT_LEQ(2, intdiv(8, 3));
  EXPECT_LEQ(3, intdiv(9, 3));
}

TEST(PointerArray, Defrag) {
  ushort **_a = null;
  ushort u;
  ushort u2;
  ulong n;

  /* All pointers null. Reduction to zero elements leaves one byte allocated.
   */
  n = 10;
  new(&_a, n);
  defrag_pointer_array(&_a, &n);
  EXPECT_ULEQ(0, n);
  deletev(&_a);

  /* One element filled, in various positions.
   */
  u = 24;
  n = 7;
  new(&_a, n);
  _a[0] = &u;
  defrag_pointer_array(&_a, &n);
  EXPECT_ULEQ(1, n);
  EXPECT_PEQ(&u, _a[0]);
  deletev(&_a);

  n = 7;
  new(&_a, n);
  _a[1] = &u;
  defrag_pointer_array(&_a, &n);
  EXPECT_ULEQ(1, n);
  EXPECT_PEQ(&u, _a[0]);
  deletev(&_a);

  n = 7;
  new(&_a, n);
  _a[6] = &u;
  defrag_pointer_array(&_a, &n);
  EXPECT_ULEQ(1, n);
  EXPECT_PEQ(&u, _a[0]);
  deletev(&_a);

  /* Two elements filled, in various positions.
   */
  u2 = 42;
  n = 7;
  new(&_a, n);
  _a[0] = &u;
  _a[1] = &u2;
  defrag_pointer_array(&_a, &n);
  EXPECT_ULEQ(2, n);
  EXPECT_PEQ(&u, _a[0]);
  EXPECT_PEQ(&u2, _a[1]);
  deletev(&_a);

  n = 7;
  new(&_a, n);
  _a[0] = &u;
  _a[6] = &u2;
  defrag_pointer_array(&_a, &n);
  EXPECT_ULEQ(2, n);
  EXPECT_PEQ(&u, _a[0]);
  EXPECT_PEQ(&u2, _a[1]);
  deletev(&_a);

  n = 7;
  new(&_a, n);
  _a[5] = &u;
  _a[6] = &u2;
  defrag_pointer_array(&_a, &n);
  EXPECT_ULEQ(2, n);
  EXPECT_PEQ(&u, _a[0]);
  EXPECT_PEQ(&u2, _a[1]);
  deletev(&_a);

  /* All elements filled.
   */
  n = 5;
  new(&_a, n);
  _a[0] = &u;
  _a[1] = &u2;
  _a[2] = &u2;
  _a[3] = &u;
  _a[4] = &u2;
  defrag_pointer_array(&_a, &n);
  EXPECT_ULEQ(5, n);
  EXPECT_PEQ(&u, _a[0]);
  EXPECT_PEQ(&u2, _a[1]);
  EXPECT_PEQ(&u2, _a[2]);
  EXPECT_PEQ(&u, _a[3]);
  EXPECT_PEQ(&u2, _a[4]);
  deletev(&_a);
}

TEST(Memory, Copy) {
  byte a[] = {0x01, 0x24, 0xff, 0x80};
  byte b[] = {0x00, 0x00, 0x00, 0x00};

  copy_memory(a, b, 4);

  EXPECT_ULEQ(b[0], 0x01);
  EXPECT_ULEQ(b[1], 0x24);
  EXPECT_ULEQ(b[2], 0xff);
  EXPECT_ULEQ(b[3], 0x80);

  EXPECT_ULEQ(a[0], 0x01);
  EXPECT_ULEQ(a[1], 0x24);
  EXPECT_ULEQ(a[2], 0xff);
  EXPECT_ULEQ(a[3], 0x80);
}

TEST(Values, Copy) {
  int a[] = {1234, -56789, 345678, -45678901};
  int b[] = {0, 0, 0, 0};

  copy_values(a, b, 4);

  EXPECT_LEQ(b[0], 1234);
  EXPECT_LEQ(b[1], -56789);
  EXPECT_LEQ(b[2], 345678);
  EXPECT_LEQ(b[3], -45678901);

  EXPECT_LEQ(a[0], 1234);
  EXPECT_LEQ(a[1], -56789);
  EXPECT_LEQ(a[2], 345678);
  EXPECT_LEQ(a[3], -45678901);
}

TEST(Values, CopyMatrix) {
  int a[2][3] = {{1234, -56789, 2409}, {345678, -45678901, 707}};
  int b[2][3] = {{0, 0, 0}, {0, 0, 0}};
  int **_a = null;
  int **_b = null;
  ulong i;
  ulong j;

  newm(&_a, 2, 3);
  newm(&_b, 2, 3);

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      _a[i][j] = a[i][j];
      _b[i][j] = b[i][j];
    }
  }

  copy_valuesm(_a, _b, 2, 3);

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 3; j++) {
      EXPECT_LEQ(_a[i][j], a[i][j]);
      EXPECT_LEQ(_b[i][j], a[i][j]);
    }
  }

  EXPECT_TRUE(values_equalm(_a, _b, 2, 3));
  _b[1][2]++;
  EXPECT_FALSE(values_equalm(_a, _b, 2, 3));

  deletemv(&_b, 2);
  deletemv(&_a, 2);
}

TEST(Values, CopyMatrixFlatten) {
  int a[2][3] = {{1234, -56789, 2409}, {345678, -45678901, 707}};
  int b[6] = {0, 0, 0, 0, 0, 0};
  int **_a = null;
  int *_b = null;
  ulong i;
  ulong j;
  ulong k;

  newm(&_a, 2, 3);
  new(&_b, 6);

  for (i = 0, k = 0; i < 2; i++) {
    for (j = 0; j < 3; j++, k++) {
      _a[i][j] = a[i][j];
      _b[k] = b[k];
    }
  }

  copy_valuesm_flatten(_a, _b, 2, 3);

  for (i = 0, k = 0; i < 2; i++) {
    for (j = 0; j < 3; j++, k++) {
      EXPECT_LEQ(_a[i][j], a[i][j]);
      EXPECT_LEQ(_b[k], a[i][j]);
    }
  }

  deletev(&_b);
  deletemv(&_a, 2);
}

TEST(Memory, Move) {
  byte a[] = {0x01, 0x24, 0xff, 0x80};
  byte b[] = {0x00, 0x00, 0x00, 0x00};

  move_memory(a, b, 4);

  EXPECT_ULEQ(b[0], 0x01);
  EXPECT_ULEQ(b[1], 0x24);
  EXPECT_ULEQ(b[2], 0xff);
  EXPECT_ULEQ(b[3], 0x80);

  EXPECT_ULEQ(a[0], 0x00);
  EXPECT_ULEQ(a[1], 0x00);
  EXPECT_ULEQ(a[2], 0x00);
  EXPECT_ULEQ(a[3], 0x00);
}

TEST(Memory, Equals) {
  byte a[] = {0x01, 0x24, 0xff, 0x80};
  byte b[] = {0x00, 0x24, 0x00, 0x00};

  EXPECT_TRUE(memory_equals(a, a, 1));
  EXPECT_TRUE(memory_equals(b, b, 1));
  EXPECT_FALSE(memory_equals(a, b, 1));

  EXPECT_TRUE(memory_equals(a + 1, a + 1, 1));
  EXPECT_TRUE(memory_equals(b + 1, b + 1, 1));
  EXPECT_TRUE(memory_equals(a + 1, b + 1, 1));

  EXPECT_TRUE(memory_equals(a + 1, a + 1, 2));
  EXPECT_TRUE(memory_equals(b + 1, b + 1, 2));
  EXPECT_FALSE(memory_equals(a + 1, b + 1, 2));

  EXPECT_TRUE(memory_equals(a, a, 4));
  EXPECT_TRUE(memory_equals(b, b, 4));
  EXPECT_FALSE(memory_equals(a, b, 4));
}

TEST(Values, Move) {
  int a[] = {1234, -56789, 345678, -45678901};
  int b[] = {0, 0, 0, 0};

  move_values(a, b, 4);

  EXPECT_LEQ(b[0], 1234);
  EXPECT_LEQ(b[1], -56789);
  EXPECT_LEQ(b[2], 345678);
  EXPECT_LEQ(b[3], -45678901);

  EXPECT_ULEQ(a[0], 0);
  EXPECT_ULEQ(a[1], 0);
  EXPECT_ULEQ(a[2], 0);
  EXPECT_ULEQ(a[3], 0);
}

TEST(Memory, Clear) {
  byte a[] = {0x01, 0x24, 0xff, 0x80};

  clear_memory(a, 1);

  EXPECT_ULEQ(a[0], 0x00);
  EXPECT_ULEQ(a[1], 0x24);
  EXPECT_ULEQ(a[2], 0xff);
  EXPECT_ULEQ(a[3], 0x80);

  clear_memory(a, 3);

  EXPECT_ULEQ(a[0], 0x00);
  EXPECT_ULEQ(a[1], 0x00);
  EXPECT_ULEQ(a[2], 0x00);
  EXPECT_ULEQ(a[3], 0x80);

  a[0] = 0xfa;
  clear_memory(a, 4);

  EXPECT_ULEQ(a[0], 0x00);
  EXPECT_ULEQ(a[1], 0x00);
  EXPECT_ULEQ(a[2], 0x00);
  EXPECT_ULEQ(a[3], 0x00);
}

TEST(Values, Clear) {
  int a[] = {1234, -56789, 345678, -45678901};

  clear_values(a, 1);

  EXPECT_LEQ(a[0], 0);
  EXPECT_LEQ(a[1], -56789);
  EXPECT_LEQ(a[2], 345678);
  EXPECT_LEQ(a[3], -45678901);

  clear_values(a, 3);

  EXPECT_LEQ(a[0], 0);
  EXPECT_LEQ(a[1], 0);
  EXPECT_LEQ(a[2], 0);
  EXPECT_LEQ(a[3], -45678901);

  a[0] = 5643;
  clear_values(a, 4);

  EXPECT_LEQ(a[0], 0);
  EXPECT_LEQ(a[1], 0);
  EXPECT_LEQ(a[2], 0);
  EXPECT_LEQ(a[3], 0);
}

TEST(Architecture, LittleEndian) {
  /* 2023-04-05: I finally realized that big-endian is dead. :) So I'm taking
   * out all the endian-independent code. Note to self: return to the backups
   * for this date if big-endian is ever needed in the future.
   *
   * So now fail if the machine is not little-endian.
   */
  ASSERT_TRUE(little_endian());
}

FINISH
