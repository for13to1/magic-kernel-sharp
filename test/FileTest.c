#include "FileTest.h"

#define N_ (64 * 1024 + 123)

static byte hash_(ulong u);


START

TEST(File, New) {
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  file_delete(&_f);
  temp_filename_delete(&_tfn);
}

TEST(File, TryNew) {
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);
  EXPECT_TRUE(file_try_new(&_f, _tfn->_filename, "w"));
  temp_filename_unlock(_tfn);
  file_delete(&_f);
  temp_filename_delete(&_tfn);

  EXPECT_FALSE(file_try_new(&_f, "/etc", "w"));
}

TEST(File, NewWrapAnsifile) {
  File *_f = null;

  file_new_wrap_ansifile(&_f, stdout, "w");
  file_delete(&_f);
}

TEST(File, IsGzipped) {
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, "txt");
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_FALSE(file_is_gzipped(_f));
  file_delete(&_f);
  temp_filename_delete(&_tfn);

  temp_filename_new(&_tfn, "gz");
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_TRUE(file_is_gzipped(_f));
  file_delete(&_f);
  temp_filename_delete(&_tfn);
}

TEST(File, WriteAndReadModes) {
  byte *_data = null;
  TempFilename *_tfn = null;
  ulong u;
  File *_f = null;

  temp_filename_new(&_tfn, null);

  /* Make sure we're overwriting by creating an existing file.
   */
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  file_write_byte(_f, 'j');
  file_write_byte(_f, 'p');
  file_write_byte(_f, 'c');
  file_delete(&_f);

  /* Now open the file write-only. This should overwrite.
   */
  file_new(&_f, _tfn->_filename, "w");
  EXPECT_ULEQ(0, file_get_size(_f));
  EXPECT_ULEQ(0, file_get_position(_f));
  new(&_data, N_);

  for (u = 0; u < N_ - 10; u++) {
    EXPECT_ULEQ(u, file_get_position(_f));

    if (u % 2573 == 0) {
      /* Expensive for an external file.
       */
      EXPECT_ULEQ(u, file_get_size(_f));
    }

    file_write_byte(_f, _data[u] = hash_(u));
  }

  /* Reposition and write some different bytes.
   */
  u = 12345;
  file_set_position(_f, u);
  file_write_byte(_f, _data[u++] = 'j');
  file_write_byte(_f, _data[u++] = 'p');
  file_write_byte(_f, _data[u++] = 'c');

  /* Now extend the file to the full N bytes. The intermediate bytes should be
   * zeroed. Note that a byte must be written for the file to be extended;
   * simply repositioning past the end of the file doesn't do it.
   */
  file_set_position(_f, N_ - 1);
  file_write_byte(_f, '!');

  for (u = N_ - 10; u < N_ - 1; u++) {
    _data[u] = 0;
  }

  _data[u] = '!';
  file_delete(&_f);

  /* Reopen file for reading only.
   */
  file_new(&_f, _tfn->_filename, "r");
  EXPECT_ULEQ(N_, file_get_size(_f));
  EXPECT_ULEQ(0, file_get_position(_f));

  for (u = 0; u < N_; u++) {
    EXPECT_ULEQ(u, file_get_position(_f));

    if (u % 1237 == 0) {
      /* Expensive for an external file.
       */
      EXPECT_ULEQ(N_, file_get_size(_f));
    }

    EXPECT_ULEQ(_data[u], file_read_byte(_f));
  }

  /* Move to arbitrary position and check all is well.
   */
  u = 12345;
  file_set_position(_f, u);
  EXPECT_ULEQ(u, file_get_position(_f));

  for (; u < 12349; u++) {
    EXPECT_ULEQ(u, file_get_position(_f));

    if (u % 1233 == 0) {
      /* Expensive for an external file.
       */
      EXPECT_ULEQ(N_, file_get_size(_f));
    }

    EXPECT_ULEQ(_data[u], file_read_byte(_f));
  }
  file_delete(&_f);

  temp_filename_delete(&_tfn);
  deletev(&_data);
}

TEST(File, AppendMode) {
  byte *_data = null;
  ulong first_length;
  ulong s;
  ulong u;
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);
  new(&_data, N_);

  /* Create the first part of the file in write-only mode.
   */
  first_length = N_ / 3;
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);

  for (u = 0; u < first_length; u++) {
    file_write_byte(_f, _data[u] = hash_(u));
  }

  file_delete(&_f);

  /* Now append to it.
   */
  file_new(&_f, _tfn->_filename, "a");

  for (; u < N_ - 3; u++) {
    file_write_byte(_f, _data[u] = hash_(u));
  }

  /* Repositioning in append mode only affects the read position; writes are
   * always appends. And since we are not in update mode, we can't actually
   * read anything anyway.
   */
  s = u;
  u = 12345;
  file_set_position(_f, u);

  /* This is a read position.
   */
  EXPECT_ULEQ(u, file_get_position(_f));

  /* This is appended.
   */
  file_write_byte(_f, _data[s++] = 'j');
  file_write_byte(_f, _data[s++] = 'p');
  file_write_byte(_f, _data[s++] = 'c');
  file_delete(&_f);

  /* Reopen file in read mode, and check the contents.
   */
  file_new(&_f, _tfn->_filename, "r");
  EXPECT_TRUE(file_contents_equals_blob(_f, N_, _data));
  file_delete(&_f);

  temp_filename_delete(&_tfn);
  deletev(&_data);
}

TEST(File, ReadUpdateMode) {
  byte *_data = null;
  ulong u;
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);
  new(&_data, N_);

  /* Start the process by creating an existing file.
   */
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  u = 0;
  file_write_byte(_f, _data[u++] = 'j');
  file_write_byte(_f, _data[u++] = 'p');
  file_write_byte(_f, _data[u++] = 'c');
  file_write_byte(_f, _data[u++] = '!');
  file_write_byte(_f, _data[u++] = '?');
  file_delete(&_f);

  /* Read and update within the file.
   */
  file_new(&_f, _tfn->_filename, "r+");

  /* Read the second byte.
   */
  file_set_position(_f, 1);
  EXPECT_ULEQ('p', file_read_byte(_f));

  /* Overwrite the fourth byte.
   */
  file_set_position(_f, 3);
  file_write_byte(_f, _data[3] = 'G');

  /* Extend the file to 20 bytes. A write is needed.
   */
  file_set_position(_f, 19);
  file_write_byte(_f, _data[19] = 'W');

  /* Reread the second and fourth bytes.
   */
  file_set_position(_f, 1);
  EXPECT_ULEQ('p', file_read_byte(_f));
  file_set_position(_f, 3);
  EXPECT_ULEQ('G', file_read_byte(_f));

  /* Write out the rest of the file.
   */
  file_set_position(_f, 20);

  for (u = 20; u < N_; u++) {
    file_write_byte(_f, _data[u] = hash_(u));
  }

  /* Check the contents without needing to reopen.
   */
  EXPECT_TRUE(file_contents_equals_blob(_f, N_, _data));
  file_delete(&_f);

  temp_filename_delete(&_tfn);
  deletev(&_data);
}

TEST(File, WriteUpdateMode) {
  byte *_data = null;
  ulong s;
  ulong u;
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);
  new(&_data, N_);

  /* Make sure we're overwriting by creating an existing file.
   */
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  file_write_byte(_f, 'j');
  file_write_byte(_f, 'p');
  file_write_byte(_f, 'c');
  file_delete(&_f);

  /* Write and update within the file.
   */
  file_new(&_f, _tfn->_filename, "w+");

  /* Write out some.
   */
  for (u = 0; u < N_ / 3; u++) {
    file_write_byte(_f, _data[u] = hash_(u));
  }

  /* Go back and read some.
   */
  s = u;
  u = N_ / 6 + 123;
  file_set_position(_f, u);
  EXPECT_ULEQ(s, file_get_size(_f));
  EXPECT_ULEQ(_data[u], file_read_byte(_f));
  u++;
  EXPECT_ULEQ(u, file_get_position(_f));
  EXPECT_ULEQ(s, file_get_size(_f));
  EXPECT_ULEQ(_data[u], file_read_byte(_f));
  u++;
  EXPECT_ULEQ(u, file_get_position(_f));
  EXPECT_ULEQ(s, file_get_size(_f));

  /* Go back and change some.
   */
  u = N_ / 7;
  file_set_position(_f, u);
  file_write_byte(_f, _data[u] = 'j');
  u++;
  file_write_byte(_f, _data[u] = 'p');
  u++;
  file_write_byte(_f, _data[u] = 'c');

  /* Write the rest of the _data, except the last 10 bytes.
   */
  file_set_position(_f, N_ / 3);

  for (u = N_ / 3; u < N_ - 10; u++) {
    file_write_byte(_f, _data[u] = hash_(u));
  }

  /* Extend the last 10 bytes.
   */
  file_set_position(_f, N_ - 1);
  file_write_byte(_f, '!');

  for (u = N_ - 10; u < N_ - 1; u++) {
    _data[u] = 0;
  }

  _data[u] = '!';

  /* Check the contents without needing to reopen.
   */
  EXPECT_TRUE(file_contents_equals_blob(_f, N_, _data));
  file_delete(&_f);

  temp_filename_delete(&_tfn);
  deletev(&_data);
}

TEST(File, AppendUpdateMode) {
  byte *_data = null;
  ulong first_length;
  ulong u;
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);
  new(&_data, N_);

  /* Create the first part of the file in write-only mode.
   */
  first_length = N_ / 3;
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);

  for (u = 0; u < first_length; u++) {
    file_write_byte(_f, _data[u] = hash_(u));
  }

  file_delete(&_f);

  /* Now append to it in update mode. This is a strange ANSI mode in which all
   * writes are appends at the end of the file, but reading can occur anywhere
   * within the file.
   */
  file_new(&_f, _tfn->_filename, "a+");

  for (; u < N_ - 3; u++) {
    file_write_byte(_f, _data[u] = hash_(u));
  }

  /* Read some random bytes.
   */
  u = 12345;
  file_set_position(_f, u);
  EXPECT_ULEQ(_data[u], file_read_byte(_f));
  u++;
  EXPECT_ULEQ(_data[u], file_read_byte(_f));
  u++;
  EXPECT_ULEQ(_data[u], file_read_byte(_f));
  u++;

  /* Now append the last three bytes.
   */
  u = file_get_size(_f);
  file_write_byte(_f, _data[u] = 'j');
  u++;
  file_write_byte(_f, _data[u] = 'p');
  u++;
  file_write_byte(_f, _data[u] = 'c');
  u++;

  /* Check the contents.
   */
  EXPECT_TRUE(file_contents_equals_blob(_f, N_, _data));
  file_delete(&_f);

  temp_filename_delete(&_tfn);
  deletev(&_data);
}

TEST(File, TryReadByte) {
  byte b;
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);
  file_new(&_f, _tfn->_filename, "w+");
  temp_filename_unlock(_tfn);
  EXPECT_FALSE(file_try_read_byte(_f, &b));
  file_delete(&_f);
  temp_filename_delete(&_tfn);
}

TEST(File, Rewind) {
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);
  file_new(&_f, _tfn->_filename, "w+");
  temp_filename_unlock(_tfn);
  file_write_byte(_f, 0x34);
  file_rewind(_f);
  EXPECT_ULEQ(0x34, file_read_byte(_f));
  file_delete(&_f);
  temp_filename_delete(&_tfn);
}

TEST(File, FastForward) {
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);
  file_new(&_f, _tfn->_filename, "w+");
  temp_filename_unlock(_tfn);
  file_write_byte(_f, 0x34);
  file_rewind(_f);
  EXPECT_ULEQ(0x34, file_read_byte(_f));
  file_rewind(_f);
  file_fast_forward(_f);
  file_write_byte(_f, 0x56);
  file_rewind(_f);
  EXPECT_ULEQ(0x34, file_read_byte(_f));
  EXPECT_ULEQ(0x56, file_read_byte(_f));
  file_delete(&_f);
  temp_filename_delete(&_tfn);
}

TEST(File, Flush) {
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);
  file_new(&_f, _tfn->_filename, "w+");
  temp_filename_unlock(_tfn);
  file_write_byte(_f, 0x34);
  file_flush(_f);
  file_delete(&_f);
  temp_filename_delete(&_tfn);
}

TEST(File, Exists) {
  TempFilename *_tfn = null;
  FILE *file;

  EXPECT_FALSE(file_exists("/badname/of/file.JPC"));
  temp_filename_new(&_tfn, null);
  file = fopen(_tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_TRUE(file_exists(_tfn->_filename));
  fclose(file);
  EXPECT_TRUE(file_exists(_tfn->_filename));
  temp_filename_delete(&_tfn);
}

TEST(File, ExistsWait) {
  TempFilename *_tfn = null;
  FILE *file;

  temp_filename_new(&_tfn, null);
  file = fopen(_tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_TRUE(file_exists_wait(_tfn->_filename, 1));
  fclose(file);
  EXPECT_TRUE(file_exists(_tfn->_filename));
  file_remove(_tfn->_filename);
  temp_filename_delete(&_tfn);
}

TEST(File, PartialExists) {
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_TRUE(file_partial_exists(_tfn->_filename));
  file_delete(&_f);
  EXPECT_FALSE(file_partial_exists(_tfn->_filename));
  file_remove(_tfn->_filename);
  temp_filename_delete(&_tfn);
}

TEST(File, EitherExists) {
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_TRUE(file_either_exists(_tfn->_filename));
  file_delete(&_f);
  EXPECT_TRUE(file_either_exists(_tfn->_filename));
  file_remove(_tfn->_filename);
  temp_filename_delete(&_tfn);
}

TEST(File, IsEmpty) {
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);

  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  file_delete(&_f);
  EXPECT_TRUE(file_is_empty(_tfn->_filename));

  file_new(&_f, _tfn->_filename, "w");
  file_write_byte(_f, 0x12);
  file_delete(&_f);
  EXPECT_FALSE(file_is_empty(_tfn->_filename));

  file_remove(_tfn->_filename);
  temp_filename_delete(&_tfn);
}

TEST(File, IsWritable) {
  char *_s = null;
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);
  EXPECT_FALSE(file_exists(_tfn->_filename));
  EXPECT_TRUE(file_is_writable(_tfn->_filename));
  EXPECT_FALSE(file_exists(_tfn->_filename));

  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  file_delete(&_f);
  EXPECT_TRUE(file_exists(_tfn->_filename));
  EXPECT_TRUE(file_is_empty(_tfn->_filename));
  EXPECT_TRUE(file_is_writable(_tfn->_filename));
  EXPECT_TRUE(file_exists(_tfn->_filename));
  EXPECT_TRUE(file_is_empty(_tfn->_filename));

  file_new(&_f, _tfn->_filename, "w");
  file_write_byte(_f, 'a');
  file_delete(&_f);
  EXPECT_TRUE(file_exists(_tfn->_filename));
  EXPECT_FALSE(file_is_empty(_tfn->_filename));
  EXPECT_TRUE(file_is_writable(_tfn->_filename));
  EXPECT_TRUE(file_exists(_tfn->_filename));
  EXPECT_FALSE(file_is_empty(_tfn->_filename));
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  file_remove(_tfn->_filename);
  temp_filename_delete(&_tfn);
}

TEST(File, Create) {
  TempFilename *_tfn = null;

  temp_filename_new(&_tfn, null);
  EXPECT_FALSE(file_exists(_tfn->_filename));
  file_create(_tfn->_filename);
  temp_filename_unlock(_tfn);
  EXPECT_TRUE(file_exists(_tfn->_filename));
  file_remove(_tfn->_filename);
  temp_filename_delete(&_tfn);
}

TEST(File, TryRemove) {
  EXPECT_FALSE(file_try_remove("/dog"));
}

TEST(File, RemovePartial) {
  TempFilename *_tfn = null;
  char *_partial_filename = null;

  temp_filename_new(&_tfn, null);
  string_new_concat(&_partial_filename, _tfn->_filename, ".partial");
  EXPECT_FALSE(file_exists(_tfn->_filename));
  EXPECT_FALSE(file_exists(_partial_filename));
  file_create(_partial_filename);
  temp_filename_unlock(_tfn);
  EXPECT_FALSE(file_exists(_tfn->_filename));
  EXPECT_TRUE(file_exists(_partial_filename));
  file_remove_partial(_tfn->_filename);
  EXPECT_FALSE(file_exists(_tfn->_filename));
  EXPECT_FALSE(file_exists(_partial_filename));
  string_delete(&_partial_filename);
  temp_filename_delete(&_tfn);
}

TEST(File, RemoveBoth) {
  TempFilename *_tfn = null;
  char *_partial_filename = null;

  temp_filename_new(&_tfn, null);
  string_new_concat(&_partial_filename, _tfn->_filename, ".partial");
  EXPECT_FALSE(file_exists(_tfn->_filename));
  EXPECT_FALSE(file_exists(_partial_filename));
  file_create(_tfn->_filename);
  temp_filename_unlock(_tfn);
  file_create(_partial_filename);
  EXPECT_TRUE(file_exists(_tfn->_filename));
  EXPECT_TRUE(file_exists(_partial_filename));
  file_remove_both(_tfn->_filename);
  EXPECT_FALSE(file_exists(_tfn->_filename));
  EXPECT_FALSE(file_exists(_partial_filename));
  string_delete(&_partial_filename);
  temp_filename_delete(&_tfn);
}

TEST(File, Rename) {
  TempFilename *_tfn_1 = null;
  TempFilename *_tfn_2 = null;

  temp_filename_new(&_tfn_1, null);
  temp_filename_new(&_tfn_2, null);
  EXPECT_FALSE(file_exists(_tfn_1->_filename));
  EXPECT_FALSE(file_exists(_tfn_2->_filename));
  file_create(_tfn_1->_filename);
  temp_filename_unlock(_tfn_1);
  EXPECT_TRUE(file_exists(_tfn_1->_filename));
  EXPECT_FALSE(file_exists(_tfn_2->_filename));
  file_rename(_tfn_1->_filename, _tfn_2->_filename);
  temp_filename_unlock(_tfn_2);
  EXPECT_FALSE(file_exists(_tfn_1->_filename));
  EXPECT_TRUE(file_exists(_tfn_2->_filename));
  file_remove(_tfn_2->_filename);
  temp_filename_delete(&_tfn_2);
  temp_filename_delete(&_tfn_1);
}

TEST(BlobNew, FromFileContents) {
  byte *_blob = null;
  TempFilename *_tfn = null;
  File *_f = null;

  temp_filename_new(&_tfn, null);
  file_new(&_f, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  file_write_byte(_f, 0x12);
  file_write_byte(_f, 0x00);
  file_write_byte(_f, 0x34);
  file_delete(&_f);
  blob_new_file_contents_from_filename(&_blob, _tfn->_filename);
  file_remove(_tfn->_filename);
  temp_filename_delete(&_tfn);
  EXPECT_ULEQ(0x12, _blob[0]);
  EXPECT_ULEQ(0x00, _blob[1]);
  EXPECT_ULEQ(0x34, _blob[2]);
  deletev(&_blob);
}

TEST(File, ProgressStatus) {
  TempFilename *_tfn = null;
  char *_partial_filename = null;

  temp_filename_new(&_tfn, null);
  string_new_concat(&_partial_filename, _tfn->_filename, ".partial");
  EXPECT_LEQ(-1, file_progress_status(_tfn->_filename));
  file_create(_partial_filename);
  temp_filename_unlock(_tfn);
  EXPECT_LEQ(0, file_progress_status(_tfn->_filename));
  file_rename(_partial_filename, _tfn->_filename);
  EXPECT_LEQ(+1, file_progress_status(_tfn->_filename));
  file_remove(_tfn->_filename);
  EXPECT_LEQ(-1, file_progress_status(_tfn->_filename));
  string_delete(&_partial_filename);
  temp_filename_delete(&_tfn);
}

TEST(Files, ProgressStatus) {
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

    EXPECT_LEQ(n == 0 ? +1 : -1, files_progress_status(n, _filenames));

    for (u = 1; u < n; u++) {
      file_create(_partial_filenames[u]);
      temp_filename_unlock(_tfns[u]);
    }

    EXPECT_LEQ(
        n == 0 ? +1 : n == 1 ? -1 : 0,
        files_progress_status(n, _filenames));

    for (u = 1; u + 1 < n; u++) {
      file_rename(_partial_filenames[u], _filenames[u]);
    }

    EXPECT_LEQ(
        n == 0 ? +1 : n == 1 ? -1 : 0,
        files_progress_status(n, _filenames));

    if (n != 0) {
      file_create(_filenames[0]);

      if (n != 1) {
        file_rename(_partial_filenames[n - 1], _filenames[n - 1]);
      }
    }

    EXPECT_LEQ(+1, files_progress_status(n, _filenames));

    for (u = 0; u < n; u++) {
      file_remove(_filenames[u]);
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
  file_remove(_tfn->_filename);
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
  file_remove(_tfn->_filename);
  temp_filename_delete(&_tfn);
}

TEST(File, Memory) {
  byte b;
  byte *_data = null;
  ulong u;
  File *_f = null;

  new(&_data, N_);
  file_new_memory(&_f);

  for (u = 0; u < N_; u++) {
    file_write_byte(_f, _data[u] = hash_(u));
  }

  file_set_position(_f, u = 987);
  EXPECT_ULEQ(u, file_get_position(_f));
  _data[u] = 'j';
  file_write_byte(_f, _data[u++]);
  file_rewind(_f);
  EXPECT_ULEQ(0, file_get_position(_f));
  EXPECT_TRUE(file_try_read_byte(_f, &b));
  EXPECT_ULEQ(_data[0], b);
  EXPECT_ULEQ(N_, file_get_size(_f));
  file_fast_forward(_f);
  EXPECT_ULEQ(N_, file_get_position(_f));
  EXPECT_TRUE(file_contents_equals_blob(_f, N_, _data));
  _data[123] = 0;
  file_delete(&_f);
  deletev(&_data);
}

TEST(File, SaveLoadMatrix) {
  short **_a = null;
  short **_b = null;
  File *_f = null;

  newm(&_a, 2, 3);
  newm(&_b, 2, 3);

  _a[0][0] = 123;
  _a[0][1] = -24;
  _a[0][2] = 9;
  _a[1][0] = -1966;
  _a[1][1] = 707;
  _a[1][2] = 1968;

  file_new_memory(&_f);
  save_valuesm(_a, 2, 3, _f);
  file_rewind(_f);
  load_valuesm(_b, 2, 3, _f);
  EXPECT_TRUE(values_equalm(_a, _b, 2, 3));
  deletemv(&_b, 2);
  deletemv(&_a, 2);
  file_delete(&_f);
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
