#include "MemoryFileTest.h"

#define N_ 300
#define P_ 8

static byte hash_(ulong u);


START

TEST(MemoryFile, New) {
  byte b;
  MemoryFile *_mf = null;

  memory_file_new(&_mf);

  /* Empty file.
   */
  EXPECT_ULEQ(0, memory_file_get_size(_mf));
  EXPECT_ULEQ(0, memory_file_get_position(_mf));

  /* Write first byte.
   */
  memory_file_write_byte(_mf, 'j');
  EXPECT_ULEQ(1, memory_file_get_size(_mf));
  EXPECT_ULEQ(1, memory_file_get_position(_mf));
  memory_file_set_position(_mf, 0);
  EXPECT_ULEQ(0, memory_file_get_position(_mf));
  EXPECT_ULEQ('j', memory_file_read_byte(_mf));
  EXPECT_FALSE(memory_file_try_read_byte(_mf, &b));
  memory_file_set_position(_mf, 1);

  /* Write second byte.
   */
  memory_file_write_byte(_mf, 'p');
  EXPECT_ULEQ(2, memory_file_get_size(_mf));
  EXPECT_ULEQ(2, memory_file_get_position(_mf));
  memory_file_set_position(_mf, 0);
  EXPECT_ULEQ(0, memory_file_get_position(_mf));
  EXPECT_ULEQ('j', memory_file_read_byte(_mf));
  EXPECT_ULEQ(1, memory_file_get_position(_mf));
  EXPECT_ULEQ('p', memory_file_read_byte(_mf));
  memory_file_set_position(_mf, 2);
  EXPECT_FALSE(memory_file_try_read_byte(_mf, &b));
  memory_file_set_position(_mf, 1);
  EXPECT_ULEQ('p', memory_file_read_byte(_mf));
  memory_file_set_position(_mf, 0);
  EXPECT_ULEQ('j', memory_file_read_byte(_mf));

  /* Write third byte.
   */
  memory_file_fast_forward(_mf);
  memory_file_write_byte(_mf, 'c');
  EXPECT_ULEQ(3, memory_file_get_size(_mf));
  EXPECT_ULEQ(3, memory_file_get_position(_mf));
  memory_file_set_position(_mf, 0);
  EXPECT_ULEQ(0, memory_file_get_position(_mf));
  EXPECT_ULEQ('j', memory_file_read_byte(_mf));
  EXPECT_ULEQ(1, memory_file_get_position(_mf));
  EXPECT_ULEQ('p', memory_file_read_byte(_mf));
  EXPECT_ULEQ(2, memory_file_get_position(_mf));
  EXPECT_ULEQ('c', memory_file_read_byte(_mf));
  memory_file_set_position(_mf, 3);
  EXPECT_FALSE(memory_file_try_read_byte(_mf, &b));
  memory_file_set_position(_mf, 2);
  EXPECT_ULEQ('c', memory_file_read_byte(_mf));
  memory_file_set_position(_mf, 1);
  EXPECT_ULEQ('p', memory_file_read_byte(_mf));
  memory_file_set_position(_mf, 0);
  EXPECT_ULEQ('j', memory_file_read_byte(_mf));

  /* Overwrite second byte.
   */
  memory_file_set_position(_mf, 1);
  memory_file_write_byte(_mf, 'h');
  memory_file_rewind(_mf);
  EXPECT_ULEQ('j', memory_file_read_byte(_mf));
  EXPECT_ULEQ('h', memory_file_read_byte(_mf));
  EXPECT_ULEQ('c', memory_file_read_byte(_mf));

  /* Overwrite first byte.
   */
  memory_file_rewind(_mf);
  memory_file_write_byte(_mf, 'm');
  memory_file_rewind(_mf);
  EXPECT_ULEQ('m', memory_file_read_byte(_mf));
  EXPECT_ULEQ('h', memory_file_read_byte(_mf));
  EXPECT_ULEQ('c', memory_file_read_byte(_mf));

  /* Check its contents, and independently that of the original file.
   */
  memory_file_rewind(_mf);
  EXPECT_ULEQ('m', memory_file_read_byte(_mf));
  EXPECT_ULEQ('h', memory_file_read_byte(_mf));
  EXPECT_ULEQ('c', memory_file_read_byte(_mf));

  /* Check the original.
   */
  memory_file_rewind(_mf);
  EXPECT_ULEQ('m', memory_file_read_byte(_mf));
  EXPECT_ULEQ('h', memory_file_read_byte(_mf));
  EXPECT_ULEQ('c', memory_file_read_byte(_mf));

  memory_file_delete(&_mf);
}

TEST(MemoryFile, ReadWriteBytes) {
  byte *_bs = null;
  ulong u;
  MemoryFile *_mf = null;

  new(&_bs, N_);

  for (u = 0; u < N_; u++) {
    _bs[u] = hash_(u);
  }

  memory_file_new(&_mf);
  memory_file_write_bytes(_mf, _bs, N_);
  memory_file_rewind(_mf);

  deletev(&_bs);
  new(&_bs, N_);

  memory_file_read_bytes(_mf, _bs, N_);
  memory_file_delete(&_mf);

  for (u = 0; u < N_; u++) {
    EXPECT_ULEQ(hash_(u), _bs[u]);
  }

  deletev(&_bs);
}

TEST(MemoryFile, Print) {
  MemoryFile *_mf = null;

  memory_file_new(&_mf);
  memory_file_write_byte(_mf, 0x12);
  memory_file_write_byte(_mf, 0x34);
  memory_file_set_position(_mf, 1);
  memory_file_delete(&_mf);
}

TEST(MemoryFile, ContentsEqualsBlob) {
  byte b;
  byte *_bytes = null;
  ulong n;
  ulong u;
  MemoryFile *_mf = null;

  n = 1024 * 1024;

  new(&_bytes, n);
  memory_file_new(&_mf);

  for (u = 0; u < n; u++) {
    b = hash_(u);
    _bytes[u] = hash_(u);
    memory_file_write_byte(_mf, b);
  }

  EXPECT_TRUE(memory_file_contents_equals_blob(_mf, n, _bytes));
  memory_file_assert_contents_equals_blob(_mf, n, _bytes);
  memory_file_delete(&_mf);
  deletev(&_bytes);
}

TEST(MemoryFile, Extend) {
  ulong p;
  ulong s;
  MemoryFile *_mf = null;

  memory_file_new(&_mf);
  EXPECT_ULEQ(0, memory_file_get_size(_mf));
  EXPECT_ULEQ(0, memory_file_get_position(_mf));
  p = 1;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 7;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  s = p;
  p = 1;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(s, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 6;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(s, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0x10;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  s = p;
  p = 0xf;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(s, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0x1f;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0x25;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0x3e;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0x41;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0x123;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0x247;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0x603;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0xfff;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0x1234;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0x37ae;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0x7fa0;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0xffff;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));

  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  s = p;
  p = 0xf;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(s, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0xffff;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(s, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0x30104;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  p = 0x90210;
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  /* Start again to test direct jumps.
   */
  p = 0x10;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x11;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x1f;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x20;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x21;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x3f;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x40;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x41;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x7f;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x7fff;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x8000;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x8001;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0xffff;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x10000;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x10001;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x1ffff;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x20000;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x20001;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x2ffff;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x30000;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);

  p = 0x30001;
  memory_file_new(&_mf);
  memory_file_set_position(_mf, p);
  EXPECT_ULEQ(p, memory_file_get_size(_mf));
  EXPECT_ULEQ(p, memory_file_get_position(_mf));
  memory_file_delete(&_mf);
}

TEST(MemoryFile, Comprehensive) {
  byte max_n_width;
  byte min_n_width;
  byte nwidth;
  byte *_data = null;
  int i;
  ulong n;
  ulong u;
  MemoryFile *_mf = null;
  Random *_random = null;

  random_new(&_random);

  for (i = 0; i < 100; i++) {
    memory_file_new(&_mf);
    max_n_width = 19;
    min_n_width = 2;
    nwidth = random_modulo(_random, max_n_width - min_n_width + 1)
      + min_n_width;
    n = random_modulo(_random, 1UL << nwidth);
    new(&_data, n);

    /* Write out a file full of data.
     */
    for (u = 0; u < n; u++) {
      memory_file_write_byte(_mf, _data[u] = random_modulo(_random, 256));
    }

    /* Read it all and check it.
     */
    memory_file_rewind(_mf);

    for (u = 0; u < n; u++) {
      EXPECT_ULEQ(_data[u], memory_file_read_byte(_mf));
    }

    /* Only do the following if there are entries.
     */
    if (n > 0) {
      /* Randomly rewrite entries, and check those either side where they
       * exist.
       */
      int j;

      for (j = 0; j < 1000; j++) {
        ulong target = random_modulo(_random, n);
        _data[target] = random_modulo(_random, 256);
        memory_file_set_position(_mf, target);
        memory_file_write_byte(_mf, _data[target]);

        if (target > 0) {
          memory_file_set_position(_mf, target - 1);
          EXPECT_ULEQ(_data[target - 1], memory_file_read_byte(_mf));
        }

        memory_file_set_position(_mf, target);
        EXPECT_ULEQ(_data[target], memory_file_read_byte(_mf));

        if (target < n - 1) {
          memory_file_set_position(_mf, target + 1);
          EXPECT_ULEQ(_data[target + 1], memory_file_read_byte(_mf));
        }
      }
    }

    deletev(&_data);
    memory_file_delete(&_mf);
  }

  random_delete(&_random);
}

TEST(MemoryFile, StringNewContents) {
  char *_s = null;
  MemoryFile *_mf = null;

  memory_file_new(&_mf);
  memory_file_write_byte(_mf, 'd');
  memory_file_write_byte(_mf, 'o');
  memory_file_write_byte(_mf, 'g');
  string_new_memory_file_contents(&_s, _mf);
  memory_file_delete(&_mf);
  EXPECT_STREQ("dog", _s);
  string_delete(&_s);
}

TEST(MemoryFile, Swappable) {
  byte w1;
  byte w2;
  byte *_bytes = null;
  byte *_bytes_1 = null;
  byte *_bytes_2 = null;
  ulong iteration;
  ulong max;
  ulong n1;
  ulong n2;
  ulong u;
  MemoryFile *_mf1 = null;
  MemoryFile *_mf2 = null;
  Random *_random = null;
  SwappableValues *_swappable = null;

  random_new(&_random);

  for (iteration = 0; iteration < N_; iteration++) {
    w1 = random_modulo(_random, P_);
    n1 = random_modulo(_random, 1 << w1);
    w2 = random_modulo(_random, P_);
    n2 = random_modulo(_random, 1 << w2);

    if (n1 == 0 && n2 == 0) {
      continue;
    }

    new(&_bytes_1, n1);
    new(&_bytes_2, n2);

    for (u = 0; u < n1; u++) {
      _bytes_1[u] = random_modulo(_random, 256);
    }

    for (u = 0; u < n2; u++) {
      _bytes_2[u] = random_modulo(_random, 256);
    }

    swappable_set_in_memory_limit(max = maximum(n1, n2));

    memory_file_new(&_mf1);
    memory_file_new(&_mf2);

    for (u = 0; u < n1; u++) {
      memory_file_write_byte(_mf1, _bytes_1[u]);
    }

    for (u = 0; u < n2; u++) {
      memory_file_write_byte(_mf2, _bytes_2[u]);
    }

    swappable_bytes_new(&_swappable, max);

    memory_file_release(_mf1);
    EXPECT_TRUE(swappable_values_is_in_memory(_mf1->_swappable));
    memory_file_release(_mf2);
    EXPECT_TRUE(
        (n1 + n2 > max
        ? swappable_values_is_in_storage
        : swappable_values_is_in_memory)(
          _mf1->_swappable));
    EXPECT_TRUE(swappable_values_is_in_memory(_mf2->_swappable));
    swappable_values_access(_swappable, &_bytes);
    EXPECT_TRUE(swappable_values_is_in_storage(_mf1->_swappable));
    EXPECT_TRUE(
        (n2 > 0
        ? swappable_values_is_in_storage
        : swappable_values_is_in_memory)(
          _mf2->_swappable));
    swappable_values_release(_swappable, &_bytes);
    memory_file_reacquire(_mf1);
    memory_file_reacquire(_mf2);

    memory_file_rewind(_mf1);
    memory_file_rewind(_mf2);

    for (u = 0; u < n1; u++) {
      EXPECT_ULEQ(_bytes_1[u], memory_file_read_byte(_mf1));
    }

    for (u = 0; u < n2; u++) {
      EXPECT_ULEQ(_bytes_2[u], memory_file_read_byte(_mf2));
    }

    swappable_values_delete(&_swappable);
    memory_file_delete(&_mf2);
    memory_file_delete(&_mf1);
    deletev(&_bytes_2);
    deletev(&_bytes_1);
  }

  random_delete(&_random);
}

FINISH


static byte hash_(ulong u) {
  byte b[16];
  union {
    ulong u;
    byte b[8];
  } converter;

  converter.u = u;
  md5_bytes(8, converter.b, b);
  return b[0];
}
