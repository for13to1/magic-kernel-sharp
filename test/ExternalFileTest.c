#include "ExternalFileTest.h"

#define N_ (64 * 1024 + 123)

static byte hash_(ulong u);


START

TEST(ExternalFile, New) {
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  external_file_delete(&_ef);
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, TryNew) {
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);
  EXPECT_TRUE(external_file_try_new(&_ef, _tfn->_filename, "w"));
  temp_filename_unlock(_tfn);
  external_file_delete(&_ef);
  temp_filename_delete(&_tfn);

  EXPECT_FALSE(external_file_try_new(&_ef, "/etc", "w"));
}

TEST(ExternalFile, NewWrapAnsifile) {
  ExternalFile *_ef = null;

  external_file_new_wrap_ansifile(&_ef, stdout, "w");
  external_file_delete(&_ef);
}

TEST(ExternalFile, IsGzipped) {
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, "txt");
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_FALSE(external_file_is_gzipped(_ef));
  external_file_delete(&_ef);
  temp_filename_delete(&_tfn);

  temp_filename_new(&_tfn, "gz");
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_TRUE(external_file_is_gzipped(_ef));
  external_file_delete(&_ef);
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, WriteAndReadModes) {
  byte *_data = null;
  ulong u;
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);

  /* Make sure we're overwriting by creating an existing file.
   */
  external_file_new(&_ef, _tfn->_filename, "w");
  external_file_write_byte(_ef, 'j');
  external_file_write_byte(_ef, 'p');
  external_file_write_byte(_ef, 'c');
  external_file_delete(&_ef);

  /* Now open the file write-only. This should overwrite.
   */
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_ULEQ(0, external_file_get_size(_ef));
  EXPECT_ULEQ(0, external_file_get_position(_ef));
  new(&_data, N_);

  for (u = 0; u < N_ - 10; u++) {
    EXPECT_ULEQ(u, external_file_get_position(_ef));

    if (u % 2573 == 0) {
      /* Expensive for an external file.
       */
      EXPECT_ULEQ(u, external_file_get_size(_ef));
    }

    external_file_write_byte(_ef, _data[u] = hash_(u));
  }

  /* Reposition and write some different bytes.
   */
  u = 12345;
  external_file_set_position(_ef, u);
  external_file_write_byte(_ef, _data[u++] = 'j');
  external_file_write_byte(_ef, _data[u++] = 'p');
  external_file_write_byte(_ef, _data[u++] = 'c');

  /* Now extend the file to the full N bytes. The intermediate bytes should be
   * zeroed. Note that a byte must be written for the file to be extended;
   * simply repositioning past the end of the file doesn't do it.
   */
  external_file_set_position(_ef, N_ - 1);
  external_file_write_byte(_ef, '!');

  for (u = N_ - 10; u < N_ - 1; u++) {
    _data[u] = 0;
  }

  _data[u] = '!';
  external_file_delete(&_ef);

  /* Reopen file for reading only.
   */
  external_file_new(&_ef, _tfn->_filename, "r");
  EXPECT_ULEQ(N_, external_file_get_size(_ef));
  EXPECT_ULEQ(0, external_file_get_position(_ef));

  for (u = 0; u < N_; u++) {
    EXPECT_ULEQ(u, external_file_get_position(_ef));

    if (u % 1237 == 0) {
      /* Expensive for an external file.
       */
      EXPECT_ULEQ(N_, external_file_get_size(_ef));
    }

    EXPECT_ULEQ(_data[u], external_file_read_byte(_ef));
  }

  /* Move to arbitrary position and check all is well.
   */
  u = 12345;
  external_file_set_position(_ef, u);
  EXPECT_ULEQ(u, external_file_get_position(_ef));

  for (; u < 12349; u++) {
    EXPECT_ULEQ(u, external_file_get_position(_ef));

    if (u % 1233 == 0) {
      /* Expensive for an external file.
       */
      EXPECT_ULEQ(N_, external_file_get_size(_ef));
    }

    EXPECT_ULEQ(_data[u], external_file_read_byte(_ef));
  }

  external_file_delete(&_ef);

  temp_filename_delete(&_tfn);
  deletev(&_data);
}

TEST(ExternalFile, AppendMode) {
  byte *_data = null;
  ulong first_length;
  ulong s;
  ulong u;
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);
  new(&_data, N_);

  /* Create the first part of the file in write-only mode.
   */
  first_length = N_ / 3;
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);

  for (u = 0; u < first_length; u++) {
    external_file_write_byte(_ef, _data[u] = hash_(u));
  }

  external_file_delete(&_ef);

  /* Now append to it.
   */
  external_file_new(&_ef, _tfn->_filename, "a");

  for (; u < N_ - 3; u++) {
    external_file_write_byte(_ef, _data[u] = hash_(u));
  }

  /* Repositioning in append mode only affects the read position; writes are
   * always appends. And since we are not in update mode, we can't actually
   * read anything anyway.
   */
  s = u;
  u = 12345;
  external_file_set_position(_ef, u);

  /* This is a read position.
   */
  EXPECT_ULEQ(u, external_file_get_position(_ef));

  /* This is appended.
   */
  external_file_write_byte(_ef, _data[s++] = 'j');
  external_file_write_byte(_ef, _data[s++] = 'p');
  external_file_write_byte(_ef, _data[s++] = 'c');
  external_file_delete(&_ef);

  /* Reopen file in read mode, and check the contents.
   */
  external_file_new(&_ef, _tfn->_filename, "r");
  EXPECT_TRUE(external_file_contents_equals_blob(_ef, N_, _data));
  external_file_delete(&_ef);

  temp_filename_delete(&_tfn);
  deletev(&_data);
}

TEST(ExternalFile, ReadUpdateMode) {
  byte *_data = null;
  ulong u;
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);
  new(&_data, N_);

  /* Start the process by creating an existing file.
   */
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  u = 0;
  external_file_write_byte(_ef, _data[u++] = 'j');
  external_file_write_byte(_ef, _data[u++] = 'p');
  external_file_write_byte(_ef, _data[u++] = 'c');
  external_file_write_byte(_ef, _data[u++] = '!');
  external_file_write_byte(_ef, _data[u++] = '?');
  external_file_delete(&_ef);

  /* Read and update within the file.
   */
  external_file_new(&_ef, _tfn->_filename, "r+");

  /* Read the second byte.
   */
  external_file_set_position(_ef, 1);
  EXPECT_ULEQ('p', external_file_read_byte(_ef));

  /* Overwrite the fourth byte.
   */
  external_file_set_position(_ef, 3);
  external_file_write_byte(_ef, _data[3] = 'G');

  /* Extend the file to 20 bytes. A write is needed.
   */
  external_file_set_position(_ef, 19);
  external_file_write_byte(_ef, _data[19] = 'W');

  /* Reread the second and fourth bytes.
   */
  external_file_set_position(_ef, 1);
  EXPECT_ULEQ('p', external_file_read_byte(_ef));
  external_file_set_position(_ef, 3);
  EXPECT_ULEQ('G', external_file_read_byte(_ef));

  /* Write out the rest of the file.
   */
  external_file_set_position(_ef, 20);

  for (u = 20; u < N_; u++) {
    external_file_write_byte(_ef, _data[u] = hash_(u));
  }

  /* Check the contents without needing to reopen.
   */
  EXPECT_TRUE(external_file_contents_equals_blob(_ef, N_, _data));
  external_file_delete(&_ef);

  temp_filename_delete(&_tfn);
  deletev(&_data);
}

TEST(ExternalFile, WriteUpdateMode) {
  byte *_data = null;
  ulong s;
  ulong u;
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);
  new(&_data, N_);

  /* Make sure we're overwriting by creating an existing file.
   */
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  external_file_write_byte(_ef, 'j');
  external_file_write_byte(_ef, 'p');
  external_file_write_byte(_ef, 'c');
  external_file_delete(&_ef);

  /* Write and update within the file.
   */
  external_file_new(&_ef, _tfn->_filename, "w+");

  /* Write out some.
   */
  for (u = 0; u < N_ / 3; u++) {
    external_file_write_byte(_ef, _data[u] = hash_(u));
  }

  /* Go back and read some.
   */
  s = u;
  u = N_ / 6 + 123;
  external_file_set_position(_ef, u);
  EXPECT_ULEQ(s, external_file_get_size(_ef));
  EXPECT_ULEQ(_data[u], external_file_read_byte(_ef));
  u++;
  EXPECT_ULEQ(u, external_file_get_position(_ef));
  EXPECT_ULEQ(s, external_file_get_size(_ef));
  EXPECT_ULEQ(_data[u], external_file_read_byte(_ef));
  u++;
  EXPECT_ULEQ(u, external_file_get_position(_ef));
  EXPECT_ULEQ(s, external_file_get_size(_ef));

  /* Go back and change some.
   */
  u = N_ / 7;
  external_file_set_position(_ef, u);
  external_file_write_byte(_ef, _data[u] = 'j');
  u++;
  external_file_write_byte(_ef, _data[u] = 'p');
  u++;
  external_file_write_byte(_ef, _data[u] = 'c');

  /* Write the rest of the _data, except the last 10 bytes.
   */
  external_file_set_position(_ef, N_ / 3);

  for (u = N_ / 3; u < N_ - 10; u++) {
    external_file_write_byte(_ef, _data[u] = hash_(u));
  }

  /* Extend the last 10 bytes.
   */
  external_file_set_position(_ef, N_ - 1);
  external_file_write_byte(_ef, '!');

  for (u = N_ - 10; u < N_ - 1; u++) {
    _data[u] = 0;
  }

  _data[u] = '!';

  /* Check the contents without needing to reopen.
   */
  EXPECT_TRUE(external_file_contents_equals_blob(_ef, N_, _data));
  external_file_delete(&_ef);

  temp_filename_delete(&_tfn);
  deletev(&_data);
}

TEST(ExternalFile, AppendUpdateMode) {
  byte *_data = null;
  ulong first_length;
  ulong u;
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);
  new(&_data, N_);

  /* Create the first part of the file in write-only mode.
   */
  first_length = N_ / 3;
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);

  for (u = 0; u < first_length; u++) {
    external_file_write_byte(_ef, _data[u] = hash_(u));
  }

  external_file_delete(&_ef);

  /* Now append to it in update mode. This is a strange ANSI mode in which all
   * writes are appends at the end of the file, but reading can occur anywhere
   * within the file.
   */
  external_file_new(&_ef, _tfn->_filename, "a+");

  for (; u < N_ - 3; u++) {
    external_file_write_byte(_ef, _data[u] = hash_(u));
  }

  /* Read some random bytes.
   */
  u = 12345;
  external_file_set_position(_ef, u);
  EXPECT_ULEQ(_data[u], external_file_read_byte(_ef));
  u++;
  EXPECT_ULEQ(_data[u], external_file_read_byte(_ef));
  u++;
  EXPECT_ULEQ(_data[u], external_file_read_byte(_ef));
  u++;

  /* Now append the last three bytes.
   */
  u = external_file_get_size(_ef);
  external_file_write_byte(_ef, _data[u] = 'j');
  u++;
  external_file_write_byte(_ef, _data[u] = 'p');
  u++;
  external_file_write_byte(_ef, _data[u] = 'c');
  u++;

  /* Check the contents.
   */
  EXPECT_TRUE(external_file_contents_equals_blob(_ef, N_, _data));
  external_file_delete(&_ef);

  temp_filename_delete(&_tfn);
  deletev(&_data);
}

TEST(ExternalFile, TryReadByte) {
  byte b;
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);
  external_file_new(&_ef, _tfn->_filename, "w+");
  temp_filename_unlock(_tfn);
  EXPECT_FALSE(external_file_try_read_byte(_ef, &b));
  external_file_delete(&_ef);
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, ReadWriteBytes) {
  byte iteration;
  byte *_bs = null;
  ulong u;
  TempFilename *_tfn = null;
  RandomCoin *_coin = null;
  ExternalFile *_ef = null;

  random_coin_new(&_coin);

  for (iteration = 0; iteration < 2; iteration++) {
    new(&_bs, N_);

    for (u = 0; u < N_; u++) {
      _bs[u] = hash_(u);
    }

    temp_filename_new(&_tfn, iteration == 0 ? null : "gz");

    if (random_coin_flip(_coin)) {
      external_file_new(&_ef, _tfn->_filename, "w");
      temp_filename_unlock(_tfn);
      external_file_write_bytes(_ef, _bs, N_);
      external_file_delete(&_ef);
    } else {
      external_file_write_bytes_to_filename(_tfn->_filename, _bs, N_);
      temp_filename_unlock(_tfn);
    }

    deletev(&_bs);
    new(&_bs, N_);

    if (random_coin_flip(_coin)) {
      external_file_new(&_ef, _tfn->_filename, "r");
      external_file_read_bytes(_ef, _bs, N_);
      external_file_delete(&_ef);
    } else {
      external_file_read_bytes_from_filename(_tfn->_filename, _bs, N_);
    }

    for (u = 0; u < N_; u++) {
      EXPECT_ULEQ(hash_(u), _bs[u]);
    }

    deletev(&_bs);
    temp_filename_delete(&_tfn);
  }

  random_coin_delete(&_coin);
}

TEST(ExternalFile, Rewind) {
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);
  external_file_new(&_ef, _tfn->_filename, "w+");
  temp_filename_unlock(_tfn);
  external_file_write_byte(_ef, 0x34);
  external_file_rewind(_ef);
  EXPECT_ULEQ(0x34, external_file_read_byte(_ef));
  external_file_delete(&_ef);
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, FastForward) {
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);
  external_file_new(&_ef, _tfn->_filename, "w+");
  temp_filename_unlock(_tfn);
  external_file_write_byte(_ef, 0x34);
  external_file_rewind(_ef);
  EXPECT_ULEQ(0x34, external_file_read_byte(_ef));
  external_file_rewind(_ef);
  external_file_fast_forward(_ef);
  external_file_write_byte(_ef, 0x56);
  external_file_rewind(_ef);
  EXPECT_ULEQ(0x34, external_file_read_byte(_ef));
  EXPECT_ULEQ(0x56, external_file_read_byte(_ef));
  external_file_delete(&_ef);
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, Flush) {
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);
  external_file_new(&_ef, _tfn->_filename, "w+");
  temp_filename_unlock(_tfn);
  external_file_write_byte(_ef, 0x34);
  external_file_flush(_ef);
  external_file_delete(&_ef);
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, Exists) {
  TempFilename *_tfn = null;
  FILE *file;

  EXPECT_FALSE(external_file_exists("/badname/of/file.JPC"));
  temp_filename_new(&_tfn, null);
  file = fopen(_tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_TRUE(external_file_exists(_tfn->_filename));
  fclose(file);
  EXPECT_TRUE(external_file_exists(_tfn->_filename));
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, ExistsWait) {
  TempFilename *_tfn = null;
  FILE *file;

  temp_filename_new(&_tfn, null);
  file = fopen(_tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_TRUE(external_file_exists_wait(_tfn->_filename, 1));
  fclose(file);
  EXPECT_TRUE(external_file_exists(_tfn->_filename));
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, PartialExists) {
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_TRUE(external_file_partial_exists(_tfn->_filename));
  external_file_delete(&_ef);
  EXPECT_FALSE(external_file_partial_exists(_tfn->_filename));
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, EitherExists) {
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_TRUE(external_file_either_exists(_tfn->_filename));
  external_file_delete(&_ef);
  EXPECT_TRUE(external_file_either_exists(_tfn->_filename));
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, IsEmpty) {
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);

  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  external_file_delete(&_ef);
  EXPECT_TRUE(external_file_is_empty(_tfn->_filename));

  external_file_new(&_ef, _tfn->_filename, "w");
  external_file_write_byte(_ef, 0x12);
  external_file_delete(&_ef);
  EXPECT_FALSE(external_file_is_empty(_tfn->_filename));

  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, IsWritable) {
  char *_s = null;
  TempFilename *_tfn = null;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);
  EXPECT_FALSE(external_file_exists(_tfn->_filename));
  EXPECT_TRUE(external_file_is_writable(_tfn->_filename));
  EXPECT_FALSE(external_file_exists(_tfn->_filename));

  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  external_file_delete(&_ef);
  EXPECT_TRUE(external_file_exists(_tfn->_filename));
  EXPECT_TRUE(external_file_is_empty(_tfn->_filename));
  EXPECT_TRUE(external_file_is_writable(_tfn->_filename));
  EXPECT_TRUE(external_file_exists(_tfn->_filename));
  EXPECT_TRUE(external_file_is_empty(_tfn->_filename));

  external_file_new(&_ef, _tfn->_filename, "w");
  external_file_write_byte(_ef, 'a');
  external_file_delete(&_ef);
  EXPECT_TRUE(external_file_exists(_tfn->_filename));
  EXPECT_FALSE(external_file_is_empty(_tfn->_filename));
  EXPECT_TRUE(external_file_is_writable(_tfn->_filename));
  EXPECT_TRUE(external_file_exists(_tfn->_filename));
  EXPECT_FALSE(external_file_is_empty(_tfn->_filename));
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, Create) {
  TempFilename *_tfn = null;

  temp_filename_new(&_tfn, null);
  EXPECT_FALSE(external_file_exists(_tfn->_filename));
  external_file_create(_tfn->_filename);
  temp_filename_unlock(_tfn);
  EXPECT_TRUE(external_file_exists(_tfn->_filename));
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, TryRemove) {
  EXPECT_FALSE(external_file_try_remove("/dog"));
}

TEST(ExternalFile, RemovePartial) {
  TempFilename *_tfn = null;
  char *_partial_filename = null;

  temp_filename_new(&_tfn, null);
  string_new_concat(&_partial_filename, _tfn->_filename, ".partial");
  EXPECT_FALSE(external_file_exists(_tfn->_filename));
  EXPECT_FALSE(external_file_exists(_partial_filename));
  external_file_create(_partial_filename);
  temp_filename_unlock(_tfn);
  EXPECT_FALSE(external_file_exists(_tfn->_filename));
  EXPECT_TRUE(external_file_exists(_partial_filename));
  external_file_remove_partial(_tfn->_filename);
  EXPECT_FALSE(external_file_exists(_tfn->_filename));
  EXPECT_FALSE(external_file_exists(_partial_filename));
  string_delete(&_partial_filename);
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, RemoveBoth) {
  TempFilename *_tfn = null;
  char *_partial_filename = null;

  temp_filename_new(&_tfn, null);
  string_new_concat(&_partial_filename, _tfn->_filename, ".partial");
  EXPECT_FALSE(external_file_exists(_tfn->_filename));
  EXPECT_FALSE(external_file_exists(_partial_filename));
  external_file_create(_tfn->_filename);
  external_file_create(_partial_filename);
  temp_filename_unlock(_tfn);
  EXPECT_TRUE(external_file_exists(_tfn->_filename));
  EXPECT_TRUE(external_file_exists(_partial_filename));
  external_file_remove_both(_tfn->_filename);
  EXPECT_FALSE(external_file_exists(_tfn->_filename));
  EXPECT_FALSE(external_file_exists(_partial_filename));
  string_delete(&_partial_filename);
  temp_filename_delete(&_tfn);
}

TEST(ExternalFile, Rename) {
  TempFilename *_tfn_1 = null;
  TempFilename *_tfn_2 = null;

  temp_filename_new(&_tfn_1, null);
  temp_filename_new(&_tfn_2, null);
  EXPECT_FALSE(external_file_exists(_tfn_1->_filename));
  EXPECT_FALSE(external_file_exists(_tfn_2->_filename));
  external_file_create(_tfn_1->_filename);
  temp_filename_unlock(_tfn_1);
  EXPECT_TRUE(external_file_exists(_tfn_1->_filename));
  EXPECT_FALSE(external_file_exists(_tfn_2->_filename));
  external_file_rename(_tfn_1->_filename, _tfn_2->_filename);
  temp_filename_unlock(_tfn_2);
  EXPECT_FALSE(external_file_exists(_tfn_1->_filename));
  EXPECT_TRUE(external_file_exists(_tfn_2->_filename));
  temp_filename_delete(&_tfn_2);
  temp_filename_delete(&_tfn_1);
}

TEST(BlobNew, ExternalFileContents) {
  byte *_blob = null;
  TempFilename *_tfn = null;
  ulong len;
  ExternalFile *_ef = null;

  temp_filename_new(&_tfn, null);
  external_file_new(&_ef, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  external_file_write_byte(_ef, 0x12);
  external_file_write_byte(_ef, 0x00);
  external_file_write_byte(_ef, 0x34);
  external_file_delete(&_ef);
  len = blob_new_external_file_contents_from_filename(
      &_blob,
      _tfn->_filename);
  temp_filename_delete(&_tfn);
  EXPECT_ULEQ(3, len);
  EXPECT_ULEQ(0x12, _blob[0]);
  EXPECT_ULEQ(0x00, _blob[1]);
  EXPECT_ULEQ(0x34, _blob[2]);
  deletev(&_blob);
}

TEST(ExternalFile, ProgressStatus) {
  TempFilename *_tfn = null;
  char *_partial_filename = null;

  temp_filename_new(&_tfn, null);
  string_new_concat(&_partial_filename, _tfn->_filename, ".partial");
  EXPECT_LEQ(-1, external_file_progress_status(_tfn->_filename));
  external_file_create(_partial_filename);
  temp_filename_unlock(_tfn);
  EXPECT_LEQ(0, external_file_progress_status(_tfn->_filename));
  external_file_rename(_partial_filename, _tfn->_filename);
  EXPECT_LEQ(+1, external_file_progress_status(_tfn->_filename));
  external_file_remove(_tfn->_filename);
  EXPECT_LEQ(-1, external_file_progress_status(_tfn->_filename));
  string_delete(&_partial_filename);
  temp_filename_delete(&_tfn);
}

TEST(ExternalFiles, ProgressStatus) {
  char **_filenames = null;
  char **_partial_filenames = null;
  ulong n;
  ulong u;
  TempFilename **_tfns = null;

  for (n = 0; n < 5; n++) {
    new(&_filenames, n);
    new(&_partial_filenames, n);
    new(&_tfns, n);

    for (u = 0; u < n; u++) {
      temp_filename_new(&_tfns[u], null);
      string_new_copy(&_filenames[u], _tfns[u]->_filename);
      string_new_concat(&_partial_filenames[u], _filenames[u], ".partial");
    }

    EXPECT_LEQ(n == 0 ? +1 : -1, external_files_progress_status(n, _filenames));

    for (u = 1; u < n; u++) {
      external_file_create(_partial_filenames[u]);
      temp_filename_unlock(_tfns[u]);
    }

    EXPECT_LEQ(
        n == 0 ? +1 : n == 1 ? -1 : 0,
        external_files_progress_status(n, _filenames));

    for (u = 1; u + 1 < n; u++) {
      external_file_rename(_partial_filenames[u], _filenames[u]);
    }

    EXPECT_LEQ(
        n == 0 ? +1 : n == 1 ? -1 : 0,
        external_files_progress_status(n, _filenames));

    if (n != 0) {
      external_file_create(_filenames[0]);

      if (n != 1) {
        external_file_rename(_partial_filenames[n - 1], _filenames[n - 1]);
      }
    }

    EXPECT_LEQ(+1, external_files_progress_status(n, _filenames));

    for (u = 0; u < n; u++) {
      external_file_remove(_filenames[u]);
    }

    delete(&_tfns, n, temp_filename_delete);
    delete(&_partial_filenames, n, string_delete);
    delete(&_filenames, n, string_delete);
  }
}

TEST(Ansifile, ContentsEqualsBlob) {
  char e[] = "This is a test\nYes it is.\f\v";
  FILE *file;

  file = tmpfile();
  fprintf(file, "%s", e);
  EXPECT_TRUE(ansifile_contents_equals_blob(file, string_length(e), (byte *)e));
  fclose(file);
}

TEST(AnsifileFromFilename, ContentsEqualsBlob) {
  char e[] = "This is a test\nYes it is.\f\v";
  TempFilename *_tfn = null;
  FILE *file;

  temp_filename_new(&_tfn, null);
  file = fopen(_tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  fprintf(file, "%s", e);
  fclose(file);
  EXPECT_TRUE(
      ansifile_from_filename_contents_equals_blob(
        _tfn->_filename,
        string_length(e),
        (byte *)e));
  external_file_remove(_tfn->_filename);
  temp_filename_delete(&_tfn);
}

TEST(Ansifile, ContentsEqualsString) {
  char e[] = "This is a test\nYes it is.\f\v";
  FILE *file;

  file = tmpfile();
  fprintf(file, "%s", e);
  EXPECT_TRUE(ansifile_contents_equals_string(file, e));
  fclose(file);
}

TEST(AnsifileFromFilename, ContentsEqualsString) {
  char e[] = "This is a test\nYes it is.\f\v";
  TempFilename *_tfn = null;
  FILE *file;

  temp_filename_new(&_tfn, null);
  file = fopen(_tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  fprintf(file, "%s", e);
  fclose(file);
  EXPECT_TRUE(
      ansifile_from_filename_contents_equals_string(_tfn->_filename, e));
  external_file_remove(_tfn->_filename);
  temp_filename_delete(&_tfn);
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
