#include "strng_test.h"
#include <ctype.h>
#include <string.h>

#define N_ 10000
#define P_ 12

START

TEST(String, SaveLoadNull) {
  char *_s = null;
  File *_f = null;

  file_new_memory(&_f);
  save(_s, string, _f);
  file_rewind(_f);
  load(_s, string, _f);
  EXPECT_NULL(_s);
  file_delete(&_f);
}

TEST(String, NewEmpty) {
  char *_s = null;

  string_new_empty(&_s);
  EXPECT_STREQ("", _s);
  test_save_load(char, string, _s);
  string_delete(&_s);
}

TEST(String, NewChar) {
  char *_s = null;

  string_new_char(&_s, 'p');
  EXPECT_STREQ("p", _s);
  test_save_load(char, string, _s);
  string_delete(&_s);
}

TEST(String, NewCopy) {
  char orig[] = "Hi there";
  char *_copy = null;

  string_new_copy(&_copy, orig);
  EXPECT_STREQ(orig, _copy);
  test_save_load(char, string, orig);
  test_save_load(char, string, _copy);
  string_delete(&_copy);
}

TEST(String, NewCopyLowercase) {
  char orig[] = "Hi THerE!@";
  char *_copy = null;

  string_new_copy_lowercase(&_copy, orig);
  EXPECT_STREQ("hi there!@", _copy);
  string_delete(&_copy);
}

TEST(String, NewF) {
  char *_s = null;
  char *e = "42 j 3.1";
  int n;

  /* Test random combinations just to make sure nothing obvious sticks out. The
   * maximum length is tested comprehensively below.
   */
  e = "42 j 3.1";
  string_new_f(&_s, "%d %c %.1f", 42, 'j', 3.14159);
  EXPECT_STREQ(e, _s);
  string_delete(&_s);

  e = "A42-173!1eb9=F1206-123ddJsswoofd12.300000!2.409197e+071.456700E-07"
    "-1.23457e+08+10.2345.%";
  string_new_f(
      &_s,
      "A%d-%o!%x=%X-%udd%css%sd%f!%e%E-%g+%G.%n%%",
      42,
      123,
      7865,
      987654,
      123,
      'J',
      "woof",
      12.3,
      24091966.0,
      0.00000014567,
      123456789.0,
      10.234458787,
      &n);
  EXPECT_ULEQ(87, n);
  EXPECT_STREQ(e, _s);
  string_delete(&_s);

  e = "123442 |+000024| 1|00000000499602d2|     3.140";
  string_new_f(
      &_s,
      "%-7.2d|%+.6d|% d|%016lx|%10.3f",
      123442,
      24,
      1,
      1234567890,
      3.14);
  EXPECT_STREQ(e, _s);
  test_save_load(char, string, _s);
  string_delete(&_s);
}

TEST(String, NewConcat) {
  char *_s = null;

  string_new_concat(&_s, "", "");
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_concat(&_s, "a", "");
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_concat(&_s, "", "a");
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_concat(&_s, "cat", "dog");
  EXPECT_STREQ("catdog", _s);
  string_delete(&_s);
}

TEST(String, NewConcatUntilNull) {
  char *_s = null;
  char a[] = "Hi there";
  char b[] = "big spender";
  char e[] = "Hi there - big spender!";
  char *c = null;

  new(&c, 2);
  c[0] = '!';
  c[1] = '\0';

  string_new_concat_until_null(&_s, a, " - ", b, c, null);
  EXPECT_STREQ(e, _s);
  string_delete(&_s);

  string_new_concat_until_null(&_s, null);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_delete(&c);
}

TEST(String, NewConcatArray) {
  char *_s = null;
  char a[] = "Hi there";
  char b[] = "big spender";
  char e[] = "Hi there - big spender!";
  char *c = null;
  char *ss[4];

  new(&c, 2);
  c[0] = '!';
  c[1] = '\0';

  ss[0] = a;
  ss[1] = " - ";
  ss[2] = b;
  ss[3] = c;

  string_new_concat_array(&_s, 4, ss);
  EXPECT_STREQ(e, _s);
  string_delete(&_s);

  string_delete(&c);
}

TEST(String, NewTruncate) {
  char *_s = null;

  string_new_truncate(&_s, "catastrophe", 3);
  EXPECT_STREQ("cat", _s);
  string_delete(&_s);

  string_new_truncate(&_s, "dog", 3);
  EXPECT_STREQ("dog", _s);
  string_delete(&_s);

  string_new_truncate(&_s, "gold", 0);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_truncate(&_s, "", 0);
  EXPECT_STREQ("", _s);
  string_delete(&_s);
}

TEST(String, NewShorten) {
  char *_s = null;

  string_new_shorten(&_s, "catastrophe", 8);
  EXPECT_STREQ("cat", _s);
  string_delete(&_s);

  string_new_shorten(&_s, "dog", 0);
  EXPECT_STREQ("dog", _s);
  string_delete(&_s);

  string_new_shorten(&_s, "gold", 4);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_shorten(&_s, "", 0);
  EXPECT_STREQ("", _s);
  string_delete(&_s);
}

TEST(String, NewIndent) {
  char *_s1 = null;
  char *_s2 = null;
  char *_s3 = null;

  string_new_indent(&_s1, "", 0);
  EXPECT_STREQ("", _s1);
  string_new_indent(&_s2, _s1, 1);
  EXPECT_STREQ(". ", _s2);
  string_new_indent(&_s3, _s2, 2);
  EXPECT_STREQ(". . . ", _s3);
  string_delete(&_s3);
  string_delete(&_s2);
  string_delete(&_s1);

  string_new_indent(&_s1, "a", 0);
  EXPECT_STREQ("a", _s1);
  string_new_indent(&_s2, _s1, 1);
  EXPECT_STREQ(". a", _s2);
  string_new_indent(&_s3, _s2, 2);
  EXPECT_STREQ(". . . a", _s3);
  string_delete(&_s3);
  string_delete(&_s2);
  string_delete(&_s1);

  string_new_indent(&_s1, "ab", 0);
  EXPECT_STREQ("ab", _s1);
  string_new_indent(&_s2, _s1, 1);
  EXPECT_STREQ(". ab", _s2);
  string_new_indent(&_s3, _s2, 2);
  EXPECT_STREQ(". . . ab", _s3);
  string_delete(&_s3);
  string_delete(&_s2);
  string_delete(&_s1);

  string_new_indent(&_s1, "\n", 0);
  EXPECT_STREQ("\n", _s1);
  string_new_indent(&_s2, _s1, 1);
  EXPECT_STREQ(". \n. ", _s2);
  string_new_indent(&_s3, _s2, 2);
  EXPECT_STREQ(". . . \n. . . ", _s3);
  string_delete(&_s3);
  string_delete(&_s2);
  string_delete(&_s1);

  string_new_indent(&_s1, "a\n", 0);
  EXPECT_STREQ("a\n", _s1);
  string_new_indent(&_s2, _s1, 1);
  EXPECT_STREQ(". a\n. ", _s2);
  string_new_indent(&_s3, _s2, 2);
  EXPECT_STREQ(". . . a\n. . . ", _s3);
  string_delete(&_s3);
  string_delete(&_s2);
  string_delete(&_s1);

  string_new_indent(&_s1, "a\nb", 0);
  EXPECT_STREQ("a\nb", _s1);
  string_new_indent(&_s2, _s1, 1);
  EXPECT_STREQ(". a\n. b", _s2);
  string_new_indent(&_s3, _s2, 2);
  EXPECT_STREQ(". . . a\n. . . b", _s3);
  string_delete(&_s3);
  string_delete(&_s2);
  string_delete(&_s1);

  string_new_indent(&_s1, "a\nb\nc", 0);
  EXPECT_STREQ("a\nb\nc", _s1);
  string_new_indent(&_s2, _s1, 1);
  EXPECT_STREQ(". a\n. b\n. c", _s2);
  string_new_indent(&_s3, _s2, 2);
  EXPECT_STREQ(". . . a\n. . . b\n. . . c", _s3);
  string_delete(&_s3);
  string_delete(&_s2);
  string_delete(&_s1);
}

TEST(String, NewFilenameFromPath) {
  char *_s = null;

  string_new_filename_from_path(&_s, "/path/to/file.txt");
  EXPECT_STREQ("file.txt", _s);
  string_delete(&_s);

  string_new_filename_from_path(&_s, "file.txt");
  EXPECT_STREQ("file.txt", _s);
  string_delete(&_s);

  string_new_filename_from_path(&_s, "relative/path/to//file");
  EXPECT_STREQ("file", _s);
  string_delete(&_s);

  string_new_filename_from_path(&_s, "/file");
  EXPECT_STREQ("file", _s);
  string_delete(&_s);

  string_new_filename_from_path(&_s, "no/_filename/here/");
  EXPECT_STREQ("", _s);
  string_delete(&_s);
}

TEST(String, RenewFilenameFromPath) {
  char *_s = null;

  string_new_copy(&_s, "1234567890");
  string_renew_filename_from_path(&_s, "/path/to/file.txt");
  EXPECT_STREQ("file.txt", _s);
  string_delete(&_s);

  string_new_copy(&_s, "1234");
  string_renew_filename_from_path(&_s, "file.txt");
  EXPECT_STREQ("file.txt", _s);
  string_delete(&_s);

  string_new_copy(&_s, "12345678901234567890");
  string_renew_filename_from_path(&_s, "relative/path/to//file");
  EXPECT_STREQ("file", _s);
  string_delete(&_s);

  string_new_copy(&_s, "1234");
  string_renew_filename_from_path(&_s, "/file");
  EXPECT_STREQ("file", _s);
  string_delete(&_s);

  string_new_copy(&_s, "123");
  string_renew_filename_from_path(&_s, "no/_filename/here/");
  EXPECT_STREQ("", _s);
  string_delete(&_s);
}

TEST(String, NewDirectoryFromPath) {
  char *_directory = null;

  string_new_directory_from_path(&_directory, "/path/to/file.txt");
  EXPECT_STREQ("/path/to", _directory);
  string_delete(&_directory);

  string_new_directory_from_path(&_directory,"file.txt");
  EXPECT_STREQ(".", _directory);
  string_delete(&_directory);

  string_new_directory_from_path(&_directory,"relative/path/to/file");
  EXPECT_STREQ("relative/path/to", _directory);
  string_delete(&_directory);

  string_new_directory_from_path(
      &_directory,
      "ignore//multiple///slashes////file");
  EXPECT_STREQ("ignore/multiple/slashes", _directory);
  string_delete(&_directory);

  string_new_directory_from_path(&_directory,"/file");
  EXPECT_STREQ("/", _directory);
  string_delete(&_directory);

  string_new_directory_from_path(&_directory,"no/filename/here/");
  EXPECT_STREQ("no/filename/here", _directory);
  string_delete(&_directory);

  string_new_directory_from_path(&_directory,"or/here///");
  EXPECT_STREQ("or/here", _directory);
  string_delete(&_directory);
}

TEST(String, NewStandardizeDirectory) {
  char *_s = null;

  string_new_standardize_directory(&_s, "");
  EXPECT_STREQ(".", _s);
  string_delete(&_s);

  string_new_standardize_directory(&_s, "/");
  EXPECT_STREQ("/", _s);
  string_delete(&_s);

  string_new_standardize_directory(&_s, "//");
  EXPECT_STREQ("/", _s);
  string_delete(&_s);

  string_new_standardize_directory(&_s, "path");
  EXPECT_STREQ("path", _s);
  string_delete(&_s);

  string_new_standardize_directory(&_s, "path/");
  EXPECT_STREQ("path", _s);
  string_delete(&_s);

  string_new_standardize_directory(&_s, "path//");
  EXPECT_STREQ("path", _s);
  string_delete(&_s);

  string_new_standardize_directory(&_s, "/path");
  EXPECT_STREQ("/path", _s);
  string_delete(&_s);

  string_new_standardize_directory(&_s, "//path");
  EXPECT_STREQ("/path", _s);
  string_delete(&_s);

  string_new_standardize_directory(&_s, "//path/");
  EXPECT_STREQ("/path", _s);
  string_delete(&_s);

  string_new_standardize_directory(&_s, "//path////");
  EXPECT_STREQ("/path", _s);
  string_delete(&_s);

  string_new_standardize_directory(&_s, "/path/to");
  EXPECT_STREQ("/path/to", _s);
  string_delete(&_s);

  string_new_standardize_directory(&_s, "/path/to/");
  EXPECT_STREQ("/path/to", _s);
  string_delete(&_s);

  string_new_standardize_directory(&_s, "//path//to///");
  EXPECT_STREQ("/path/to", _s);
  string_delete(&_s);
}

TEST(String, RenewStandardizeDirectory) {
  char *_s = null;

  string_new_copy(&_s, "");
  string_renew_standardize_directory(&_s);
  EXPECT_STREQ(".", _s);
  string_delete(&_s);

  string_new_copy(&_s, "/");
  string_renew_standardize_directory(&_s);
  EXPECT_STREQ("/", _s);
  string_delete(&_s);

  string_new_copy(&_s, "//");
  string_renew_standardize_directory(&_s);
  EXPECT_STREQ("/", _s);
  string_delete(&_s);

  string_new_copy(&_s, "path");
  string_renew_standardize_directory(&_s);
  EXPECT_STREQ("path", _s);
  string_delete(&_s);

  string_new_copy(&_s, "path/");
  string_renew_standardize_directory(&_s);
  EXPECT_STREQ("path", _s);
  string_delete(&_s);

  string_new_copy(&_s, "path//");
  string_renew_standardize_directory(&_s);
  EXPECT_STREQ("path", _s);
  string_delete(&_s);

  string_new_copy(&_s, "/path");
  string_renew_standardize_directory(&_s);
  EXPECT_STREQ("/path", _s);
  string_delete(&_s);

  string_new_copy(&_s, "//path");
  string_renew_standardize_directory(&_s);
  EXPECT_STREQ("/path", _s);
  string_delete(&_s);

  string_new_copy(&_s, "//path/");
  string_renew_standardize_directory(&_s);
  EXPECT_STREQ("/path", _s);
  string_delete(&_s);

  string_new_copy(&_s, "//path////");
  string_renew_standardize_directory(&_s);
  EXPECT_STREQ("/path", _s);
  string_delete(&_s);

  string_new_copy(&_s, "/path/to");
  string_renew_standardize_directory(&_s);
  EXPECT_STREQ("/path/to", _s);
  string_delete(&_s);

  string_new_copy(&_s, "/path/to/");
  string_renew_standardize_directory(&_s);
  EXPECT_STREQ("/path/to", _s);
  string_delete(&_s);

  string_new_copy(&_s, "//path//to///");
  string_renew_standardize_directory(&_s);
  EXPECT_STREQ("/path/to", _s);
  string_delete(&_s);
}

TEST(Strings, NewDirectoryFilenameFromPath) {
  char *_directory = null;
  char *_filename = null;

  strings_new_directory_filename_from_path(
      &_directory,
      &_filename,
      "/path/to/file.txt");
  EXPECT_STREQ("/path/to", _directory);
  EXPECT_STREQ("file.txt", _filename);
  string_delete(&_filename);
  string_delete(&_directory);

  strings_new_directory_filename_from_path(
      &_directory,
      &_filename,
      "file.txt");
  EXPECT_STREQ(".", _directory);
  EXPECT_STREQ("file.txt", _filename);
  string_delete(&_filename);
  string_delete(&_directory);

  strings_new_directory_filename_from_path(
      &_directory,
      &_filename,
      "relative/path/to/file");
  EXPECT_STREQ("relative/path/to", _directory);
  EXPECT_STREQ("file", _filename);
  string_delete(&_filename);
  string_delete(&_directory);

  strings_new_directory_filename_from_path(
      &_directory,
      &_filename,
      "ignore//multiple///slashes////file");
  EXPECT_STREQ("ignore/multiple/slashes", _directory);
  EXPECT_STREQ("file", _filename);
  string_delete(&_filename);
  string_delete(&_directory);

  strings_new_directory_filename_from_path(
      &_directory,
      &_filename,
      "/file");
  EXPECT_STREQ("/", _directory);
  EXPECT_STREQ("file", _filename);
  string_delete(&_filename);
  string_delete(&_directory);

  strings_new_directory_filename_from_path(
      &_directory,
      &_filename,
      "no/filename/here/");
  EXPECT_STREQ("no/filename/here", _directory);
  EXPECT_STREQ("", _filename);
  string_delete(&_filename);
  string_delete(&_directory);

  strings_new_directory_filename_from_path(
      &_directory,
      &_filename,
      "or/here///");
  EXPECT_STREQ("or/here", _directory);
  EXPECT_STREQ("", _filename);
  string_delete(&_filename);
  string_delete(&_directory);
}

TEST(String, NewExtensionFromPath) {
  char *_ = null;

  string_new_extension_from_path(&_, "");
  EXPECT_NULL(_);

  string_new_extension_from_path(&_, "dog");
  EXPECT_NULL(_);

  string_new_extension_from_path(&_, "dog.gz");
  EXPECT_NULL(_);

  string_new_extension_from_path(&_, "dog.cat/woof");
  EXPECT_NULL(_);

  string_new_extension_from_path(&_, "dog.cat/woof.gz");
  EXPECT_NULL(_);

  string_new_extension_from_path(&_, ".woof");
  EXPECT_NULL(_);

  string_new_extension_from_path(&_, ".woof.gz");
  EXPECT_NULL(_);

  string_new_extension_from_path(&_, "//a.b/.woof");
  EXPECT_NULL(_);

  string_new_extension_from_path(&_, "//a.b/.woof.gz");
  EXPECT_NULL(_);

  string_new_extension_from_path(&_, "//a.b/.");
  EXPECT_NULL(_);

  string_new_extension_from_path(&_, "//a.b/.gz");
  EXPECT_NULL(_);

  string_new_extension_from_path(&_, "a.");
  EXPECT_STREQ("", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "a..gz");
  EXPECT_STREQ("", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "a.b");
  EXPECT_STREQ("b", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "a.b.gz");
  EXPECT_STREQ("b", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "ac.b");
  EXPECT_STREQ("b", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "ac.b.gz");
  EXPECT_STREQ("b", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "acd.b");
  EXPECT_STREQ("b", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "acd.b.gz");
  EXPECT_STREQ("b", _);
  string_delete(&_);

  string_new_extension_from_path(&_, ".a.b");
  EXPECT_STREQ("b", _);
  string_delete(&_);

  string_new_extension_from_path(&_, ".a.b.gz");
  EXPECT_STREQ("b", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "c.d/.a.b");
  EXPECT_STREQ("b", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "c.d/.a.b.gz");
  EXPECT_STREQ("b", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "a.bc");
  EXPECT_STREQ("bc", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "a.bc.gz");
  EXPECT_STREQ("bc", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "ad.bc");
  EXPECT_STREQ("bc", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "ad.bc.gz");
  EXPECT_STREQ("bc", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "ade.bc");
  EXPECT_STREQ("bc", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "ade.bc.gz");
  EXPECT_STREQ("bc", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "..bc");
  EXPECT_STREQ("bc", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "..bc.gz");
  EXPECT_STREQ("bc", _);
  string_delete(&_);

  string_new_extension_from_path(&_, ".a.bc");
  EXPECT_STREQ("bc", _);
  string_delete(&_);

  string_new_extension_from_path(&_, ".a.bc.gz");
  EXPECT_STREQ("bc", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "dog/.a.bc");
  EXPECT_STREQ("bc", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "dog/.a.bc.gz");
  EXPECT_STREQ("bc", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "dog.cat/w.x//.a.bc");
  EXPECT_STREQ("bc", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "dog.cat/w.x//.a.bc.gz");
  EXPECT_STREQ("bc", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "a.cat");
  EXPECT_STREQ("cat", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "a.cat.gz");
  EXPECT_STREQ("cat", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "ab.woof");
  EXPECT_STREQ("woof", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "ab.woof.gz");
  EXPECT_STREQ("woof", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "/.a.12345");
  EXPECT_STREQ("12345", _);
  string_delete(&_);

  string_new_extension_from_path(&_, "/.a.12345.gz");
  EXPECT_STREQ("12345", _);
  string_delete(&_);
}

TEST(String, NewExtensionGzippedFromPath) {
  bool g;
  char *_ = null;

  string_new_extension_gzipped_from_path(&_, &g, "");
  EXPECT_FALSE(g);
  EXPECT_NULL(_);

  string_new_extension_gzipped_from_path(&_, &g, ".gz");
  EXPECT_TRUE(g);
  EXPECT_NULL(_);

  string_new_extension_gzipped_from_path(&_, &g, "dog");
  EXPECT_FALSE(g);
  EXPECT_NULL(_);

  string_new_extension_gzipped_from_path(&_, &g, "dog.gz");
  EXPECT_TRUE(g);
  EXPECT_NULL(_);

  string_new_extension_gzipped_from_path(&_, &g, "dog.cat/woof");
  EXPECT_FALSE(g);
  EXPECT_NULL(_);

  string_new_extension_gzipped_from_path(&_, &g, "dog.cat/woof.gz");
  EXPECT_TRUE(g);
  EXPECT_NULL(_);

  string_new_extension_gzipped_from_path(&_, &g, ".woof");
  EXPECT_FALSE(g);
  EXPECT_NULL(_);

  string_new_extension_gzipped_from_path(&_, &g, ".woof.gz");
  EXPECT_TRUE(g);
  EXPECT_NULL(_);

  string_new_extension_gzipped_from_path(&_, &g, "//a.b/.woof");
  EXPECT_FALSE(g);
  EXPECT_NULL(_);

      string_new_extension_gzipped_from_path(&_, &g, "//a.b/.woof.gz");
  EXPECT_TRUE(g);
  EXPECT_NULL(_);

  string_new_extension_gzipped_from_path(&_, &g, "//a.b/.");
  EXPECT_FALSE(g);
  EXPECT_NULL(_);

  string_new_extension_gzipped_from_path(&_, &g, "//a.b/.gz");
  EXPECT_TRUE(g);
  EXPECT_NULL(_);

  string_new_extension_gzipped_from_path(&_, &g, "a.");
  EXPECT_STREQ("", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "a..gz");
  EXPECT_STREQ("", _);
  EXPECT_TRUE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "a.b");
  EXPECT_STREQ("b", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "a.b.gz");
  EXPECT_STREQ("b", _);
  EXPECT_TRUE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "ac.b");
  EXPECT_STREQ("b", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "ac.b.gz");
  EXPECT_STREQ("b", _);
  EXPECT_TRUE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "acd.b");
  EXPECT_STREQ("b", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "acd.b.gz");
  EXPECT_STREQ("b", _);
  EXPECT_TRUE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, ".a.b");
  EXPECT_STREQ("b", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, ".a.b.gz");
  EXPECT_STREQ("b", _);
  EXPECT_TRUE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "c.d/.a.b");
  EXPECT_STREQ("b", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "c.d/.a.b.gz");
  EXPECT_STREQ("b", _);
  EXPECT_TRUE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "a.bc");
  EXPECT_STREQ("bc", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "a.bc.gz");
  EXPECT_STREQ("bc", _);
  EXPECT_TRUE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "ad.bc");
  EXPECT_STREQ("bc", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "ad.bc.gz");
  EXPECT_STREQ("bc", _);
  EXPECT_TRUE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "ade.bc");
  EXPECT_STREQ("bc", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "ade.bc.gz");
  EXPECT_STREQ("bc", _);
  EXPECT_TRUE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "..bc");
  EXPECT_STREQ("bc", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "..bc.gz");
  EXPECT_STREQ("bc", _);
  EXPECT_TRUE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, ".a.bc");
  EXPECT_STREQ("bc", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, ".a.bc.gz");
  EXPECT_STREQ("bc", _);
  EXPECT_TRUE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "dog/.a.bc");
  EXPECT_STREQ("bc", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "dog/.a.bc.gz");
  EXPECT_STREQ("bc", _);
  EXPECT_TRUE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "dog.cat/w.x//.a.bc");
  EXPECT_STREQ("bc", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "dog.cat/w.x//.a.bc.gz");
  EXPECT_STREQ("bc", _);
  EXPECT_TRUE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "a.cat");
  EXPECT_STREQ("cat", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "a.cat.gz");
  EXPECT_STREQ("cat", _);
  EXPECT_TRUE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "ab.woof");
  EXPECT_STREQ("woof", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "ab.woof.gz");
  EXPECT_STREQ("woof", _);
  EXPECT_TRUE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "/.a.12345");
  EXPECT_STREQ("12345", _);
  EXPECT_FALSE(g);
  string_delete(&_);

  string_new_extension_gzipped_from_path(&_, &g, "/.a.12345.gz");
  EXPECT_STREQ("12345", _);
  EXPECT_TRUE(g);
  string_delete(&_);
}

TEST(String, NewRemoveExtension) {
  char *_ = null;

  string_new_remove_extension(&_, "");
  EXPECT_STREQ("", _);
  string_delete(&_);

  string_new_remove_extension(&_, "a");
  EXPECT_STREQ("a", _);
  string_delete(&_);

  string_new_remove_extension(&_, "ab");
  EXPECT_STREQ("ab", _);
  string_delete(&_);

  string_new_remove_extension(&_, "a./b");
  EXPECT_STREQ("a./b", _);
  string_delete(&_);

  string_new_remove_extension(&_, "a.b/c");
  EXPECT_STREQ("a.b/c", _);
  string_delete(&_);

  string_new_remove_extension(&_, "a.b/");
  EXPECT_STREQ("a.b/", _);
  string_delete(&_);

  string_new_remove_extension(&_, ".gz");
  EXPECT_STREQ("", _);
  string_delete(&_);

  string_new_remove_extension(&_, "a.gz");
  EXPECT_STREQ("a", _);
  string_delete(&_);

  string_new_remove_extension(&_, "a..gz");
  EXPECT_STREQ("a", _);
  string_delete(&_);

  string_new_remove_extension(&_, "a...gz");
  EXPECT_STREQ("a.", _);
  string_delete(&_);

  string_new_remove_extension(&_, "a.b.gz");
  EXPECT_STREQ("a", _);
  string_delete(&_);

  string_new_remove_extension(&_, "ab.cd.gz");
  EXPECT_STREQ("ab", _);
  string_delete(&_);

  string_new_remove_extension(&_, "ab/cd.gz");
  EXPECT_STREQ("ab/cd", _);
  string_delete(&_);

  string_new_remove_extension(&_, "ab.cd/ef.gz");
  EXPECT_STREQ("ab.cd/ef", _);
  string_delete(&_);

  string_new_remove_extension(&_, "a./.gz");
  EXPECT_STREQ("a./", _);
  string_delete(&_);

  string_new_remove_extension(&_, ".file");
  EXPECT_STREQ("", _);
  string_delete(&_);

  string_new_remove_extension(&_, "a.file");
  EXPECT_STREQ("a", _);
  string_delete(&_);

  string_new_remove_extension(&_, "a..files");
  EXPECT_STREQ("a.", _);
  string_delete(&_);

  string_new_remove_extension(&_, "a...z");
  EXPECT_STREQ("a..", _);
  string_delete(&_);

  string_new_remove_extension(&_, "a.b");
  EXPECT_STREQ("a", _);
  string_delete(&_);

  string_new_remove_extension(&_, "ab.cd");
  EXPECT_STREQ("ab", _);
  string_delete(&_);

  string_new_remove_extension(&_, "ab/cd.dog.gz");
  EXPECT_STREQ("ab/cd", _);
  string_delete(&_);

  string_new_remove_extension(&_, "ab.cd/ef.dog.gz");
  EXPECT_STREQ("ab.cd/ef", _);
  string_delete(&_);

  string_new_remove_extension(&_, "a./.dog.gz");
  EXPECT_STREQ("a./", _);
  string_delete(&_);
}

TEST(String, NewTrimWhitespace) {
  char *_s = null;

  string_new_trim_whitespace(&_s, "");
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, " ");
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, "\r");
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, "  ");
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, "\r \n \f \v \t  ");
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, "a");
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, "ab");
  EXPECT_STREQ("ab", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, "abc");
  EXPECT_STREQ("abc", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, " a");
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, " ab");
  EXPECT_STREQ("ab", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, " abc");
  EXPECT_STREQ("abc", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, "a ");
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, "ab ");
  EXPECT_STREQ("ab", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, "abc ");
  EXPECT_STREQ("abc", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, " a ");
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, " a b ");
  EXPECT_STREQ("a b", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, " a b c ");
  EXPECT_STREQ("a b c", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, "\n\r a  \t ");
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, "\n  \r\f a b \r\t\v");
  EXPECT_STREQ("a b", _s);
  string_delete(&_s);

  string_new_trim_whitespace(&_s, " \r   \t a b c  \f \t  ");
  EXPECT_STREQ("a b c", _s);
  string_delete(&_s);
}

TEST(String, RenewTrimWhitespace) {
  char *_s = null;

  string_new_empty(&_s);
  string_renew_trim_whitespace(&_s);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_copy(&_s, " ");
  string_renew_trim_whitespace(&_s);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_copy(&_s, "  ");
  string_renew_trim_whitespace(&_s);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_copy(&_s, "   ");
  string_renew_trim_whitespace(&_s);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_copy(&_s, "a ");
  string_renew_trim_whitespace(&_s);
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_copy(&_s, " a");
  string_renew_trim_whitespace(&_s);
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_copy(&_s, " a ");
  string_renew_trim_whitespace(&_s);
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_copy(&_s, "  a ");
  string_renew_trim_whitespace(&_s);
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_copy(&_s, "  a b");
  string_renew_trim_whitespace(&_s);
  EXPECT_STREQ("a b", _s);
  string_delete(&_s);

  string_new_copy(&_s, "a b   ");
  string_renew_trim_whitespace(&_s);
  EXPECT_STREQ("a b", _s);
  string_delete(&_s);
}

TEST(String, NewSubstring) {
  char *_s = null;

  string_new_substring(&_s, "", 0, 0);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_substring(&_s, "a", 0, 0);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_substring(&_s, "a", 1, 0);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_substring(&_s, "ab", 0, 0);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_substring(&_s, "ab", 1, 0);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_substring(&_s, "ab", 2, 0);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_substring(&_s, "a", 0, 1);
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_substring(&_s, "ab", 0, 1);
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_substring(&_s, "ab", 1, 1);
  EXPECT_STREQ("b", _s);
  string_delete(&_s);

  string_new_substring(&_s, "ab", 0, 2);
  EXPECT_STREQ("ab", _s);
  string_delete(&_s);
}

TEST(String, NewJoin) {
  char *a[] = {"cat", "dog", "pig"};
  char *_s = null;

  string_new_join(&_s, 0, null, null);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_join(&_s, 1, a, null);
  EXPECT_STREQ("cat", _s);
  string_delete(&_s);

  string_new_join(&_s, 2, a, " & ");
  EXPECT_STREQ("cat & dog", _s);
  string_delete(&_s);

  string_new_join(&_s, 3, a, " and ");
  EXPECT_STREQ("cat and dog and pig", _s);
  string_delete(&_s);
}

TEST(String, NewReplaceChars) {
  char *_s = null;

  string_new_replace_chars(&_s, "", "", 'z');
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_replace_chars(&_s, "", "d", 'z');
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_replace_chars(&_s, "", "do", 'z');
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_replace_chars(&_s, "", "dog", 'z');
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_replace_chars(&_s, "dog", "", 'z');
  EXPECT_STREQ("dog", _s);
  string_delete(&_s);

  string_new_replace_chars(&_s, "dog", "d", 'z');
  EXPECT_STREQ("zog", _s);
  string_delete(&_s);

  string_new_replace_chars(&_s, "dog", "o", 'z');
  EXPECT_STREQ("dzg", _s);
  string_delete(&_s);

  string_new_replace_chars(&_s, "dog", "g", 'z');
  EXPECT_STREQ("doz", _s);
  string_delete(&_s);

  string_new_replace_chars(&_s, "dog", "do", 'z');
  EXPECT_STREQ("zzg", _s);
  string_delete(&_s);

  string_new_replace_chars(&_s, "dog", "dog", 'z');
  EXPECT_STREQ("zzz", _s);
  string_delete(&_s);
}

TEST(String, NewReplaceCharWithString) {
  char *_s = null;

  string_new_replace_char_with_string(&_s, "", '$', "dog");
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_replace_char_with_string(&_s, "a", '$', "dog");
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_replace_char_with_string(&_s, "ab", '$', "dog");
  EXPECT_STREQ("ab", _s);
  string_delete(&_s);

  string_new_replace_char_with_string(&_s, "$", '$', "dog");
  EXPECT_STREQ("dog", _s);
  string_delete(&_s);

  string_new_replace_char_with_string(&_s, "a$", '$', "dog");
  EXPECT_STREQ("adog", _s);
  string_delete(&_s);

  string_new_replace_char_with_string(&_s, "$a", '$', "dog");
  EXPECT_STREQ("doga", _s);
  string_delete(&_s);

  string_new_replace_char_with_string(&_s, "a$b", '$', "dog");
  EXPECT_STREQ("adogb", _s);
  string_delete(&_s);

  string_new_replace_char_with_string(&_s, "$a$", '$', "dog");
  EXPECT_STREQ("dogadog", _s);
  string_delete(&_s);

  string_new_replace_char_with_string(&_s, "$ab$", '$', "dog");
  EXPECT_STREQ("dogabdog", _s);
  string_delete(&_s);

  string_new_replace_char_with_string(&_s, "a$bc$d", '$', "dog");
  EXPECT_STREQ("adogbcdogd", _s);
  string_delete(&_s);

  string_new_replace_char_with_string(&_s, "$$", '$', "dog");
  EXPECT_STREQ("$", _s);
  string_delete(&_s);

  string_new_replace_char_with_string(&_s, "a$$", '$', "dog");
  EXPECT_STREQ("a$", _s);
  string_delete(&_s);

  string_new_replace_char_with_string(&_s, "$$a", '$', "dog");
  EXPECT_STREQ("$a", _s);
  string_delete(&_s);
}

TEST(String, NewEscape) {
  char *_s = null;

  string_new_escape(&_s, "");
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_escape(&_s, "a");
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_escape(&_s, "ab");
  EXPECT_STREQ("ab", _s);
  string_delete(&_s);

  string_new_escape(&_s, "abc");
  EXPECT_STREQ("abc", _s);
  string_delete(&_s);

  string_new_escape(&_s, "a\nbc");
  EXPECT_STREQ("a\\nbc", _s);
  string_delete(&_s);

  string_new_escape(&_s, "a\nb\ac\b\f\r\t\v\\'\"");
  EXPECT_STREQ("a\\nb\\ac\\b\\f\\r\\t\\v\\\\'\"", _s);
  string_delete(&_s);

  string_new_escape(&_s, "\x01\x02\x1f\x7f\x80\x81\xfe\xff");
  EXPECT_STREQ("\\001\\002\\037\\177\\200\\201\\376\\377", _s);
  string_delete(&_s);
}

TEST(String, NewEscapeForLogging) {
  char *_s = null;

  string_new_escape_for_logging(&_s, "");
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_escape_for_logging(&_s, "a");
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_escape_for_logging(&_s, "ab");
  EXPECT_STREQ("ab", _s);
  string_delete(&_s);

  string_new_escape_for_logging(&_s, "abc");
  EXPECT_STREQ("abc", _s);
  string_delete(&_s);

  string_new_escape_for_logging(&_s, "a\nbc");
  EXPECT_STREQ("a\nbc", _s);
  string_delete(&_s);

  string_new_escape_for_logging(&_s, "a\nb\ac\b\f\r\t\v\\'\"");
  EXPECT_STREQ("a\nb\\ac\\b\\f\\r\\t\\v\\'\"", _s);
  string_delete(&_s);

  string_new_escape_for_logging(&_s, "\x01\x02\x1f\x7f\x80\x81\xfe\xff");
  EXPECT_STREQ("\\001\\002\\037\\177\\200\\201\\376\\377", _s);
  string_delete(&_s);
}

TEST(String, RenewEscapeForLogging) {
  char *_s = null;

  string_new_copy(&_s, "");
  string_renew_escape_for_logging(&_s);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_copy(&_s, "a");
  string_renew_escape_for_logging(&_s);
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_copy(&_s, "ab");
  string_renew_escape_for_logging(&_s);
  EXPECT_STREQ("ab", _s);
  string_delete(&_s);

  string_new_copy(&_s, "abc");
  string_renew_escape_for_logging(&_s);
  EXPECT_STREQ("abc", _s);
  string_delete(&_s);

  string_new_copy(&_s, "a\nbc");
  string_renew_escape_for_logging(&_s);
  EXPECT_STREQ("a\nbc", _s);
  string_delete(&_s);

  string_new_copy(&_s, "a\nb\ac\b\f\r\t\v\\'\"");
  string_renew_escape_for_logging(&_s);
  EXPECT_STREQ("a\nb\\ac\\b\\f\\r\\t\\v\\'\"", _s);
  string_delete(&_s);

  string_new_copy(&_s, "\x01\x02\x1f\x7f\x80\x81\xfe\xff");
  string_renew_escape_for_logging(&_s);
  EXPECT_STREQ("\\001\\002\\037\\177\\200\\201\\376\\377", _s);
  string_delete(&_s);
}

TEST(String, NewUnescape) {
  char *_s = null;

  string_new_unescape(&_s, "");
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_unescape(&_s, "a");
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_unescape(&_s, "ab");
  EXPECT_STREQ("ab", _s);
  string_delete(&_s);

  string_new_unescape(&_s, "abc");
  EXPECT_STREQ("abc", _s);
  string_delete(&_s);

  string_new_unescape(&_s, "ab\\nc");
  EXPECT_STREQ("ab\nc", _s);
  string_delete(&_s);

  string_new_unescape(&_s, "a\\nb\\tc\\vd\\be\\rf\\fg\\ah");
  EXPECT_STREQ("a\nb\tc\vd\be\rf\fg\ah", _s);
  string_delete(&_s);

  string_new_unescape(&_s, "\\\\\\?\\'\\\"");
  EXPECT_STREQ("\\?'\"", _s);
  string_delete(&_s);

  string_new_unescape(&_s, "\\01\\03\\07\\1\\2\\7\\001\\377");
  EXPECT_STREQ("\1\3\7\1\2\7\1\377", _s);
  string_delete(&_s);

  string_new_unescape(&_s, "\\12\\012\\70\\070");
  EXPECT_STREQ("\12\12\70\70", _s);
  string_delete(&_s);

  string_new_unescape(&_s, "\\x1\\x01\\x9\\xa\\x10\\xff");
  EXPECT_STREQ("\x1\x1\x9\xa\x10\xff", _s);
  string_delete(&_s);
}

TEST(String, EscapeUnescapeComprehensive) {
  char c;
  char *p;
  char *_escaped = null;
  char *_orig = null;
  char *_unescaped = null;
  ulong length;
  ulong length_width;
  ulong u;
  ulong v;
  Random *_random = null;

  random_new(&_random);

  for (u = 0; u < N_; u++) {
    length_width = random_modulo(_random, P_);
    length = random_modulo(_random, 1 << length_width);
    new(&_orig, length + 1);

    for (v = 0; v < length; v++) {
      _orig[v] = 1 + random_modulo(_random, 255);
    }

    _orig[v] = '\0';

    string_new_escape(&_escaped, _orig);

    for (p = _escaped; (c = *p) != '\0'; p++) {
      if (!isprint(c)) {
        throw("Non-printable character 0x%02x found", c);
      }
    }

    string_new_unescape(&_unescaped, _escaped);

    EXPECT_STREQ(_orig, _unescaped);

    string_delete(&_unescaped);
    string_delete(&_escaped);
    string_delete(&_orig);
  }

  random_delete(&_random);
}

TEST(String, NewEscapeChar) {
  char *_s = null;

  string_new_escape_char(&_s, 'a');
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_escape_char(&_s, '\t');
  EXPECT_STREQ("\\t", _s);
  string_delete(&_s);

  string_new_escape_char(&_s, '\x01');
  EXPECT_STREQ("\\001", _s);
  string_delete(&_s);
}

TEST(String, NewQuoteOrNull) {
  char *_s = null;

  string_new_quote_or_null(&_s, null, "!woof?");
  EXPECT_STREQ("!woof?", _s);
  string_delete(&_s);

  string_new_quote_or_null(&_s, "dog", "!?");
  EXPECT_STREQ("'dog'", _s);
  string_delete(&_s);
}

TEST(String, NewAddSeparators) {
  char *_s = null;

  string_new_add_separators(&_s, "");
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "a");
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "ab");
  EXPECT_STREQ("ab", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "abc");
  EXPECT_STREQ("abc", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "abcd");
  EXPECT_STREQ("abcd", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "abcde");
  EXPECT_STREQ("abcde", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "0");
  EXPECT_STREQ("0", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "12");
  EXPECT_STREQ("12", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "123");
  EXPECT_STREQ("123", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "1234");
  EXPECT_STREQ("1234", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "12345");
  EXPECT_STREQ("12,345", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "123456");
  EXPECT_STREQ("123,456", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "a 0");
  EXPECT_STREQ("a 0", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "a-123456");
  EXPECT_STREQ("a-123456", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "! 12");
  EXPECT_STREQ("! 12", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "= 123");
  EXPECT_STREQ("= 123", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "a 1234");
  EXPECT_STREQ("a 1234", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "= 12345");
  EXPECT_STREQ("= 12,345", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "> 123456");
  EXPECT_STREQ("> 123,456", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "-1234567");
  EXPECT_STREQ("-1,234,567", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "a0");
  EXPECT_STREQ("a0", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "a12");
  EXPECT_STREQ("a12", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "a123");
  EXPECT_STREQ("a123", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "a1234");
  EXPECT_STREQ("a1234", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "a12345");
  EXPECT_STREQ("a12345", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "a123456");
  EXPECT_STREQ("a123456", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "A1234567");
  EXPECT_STREQ("A1234567", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "0a");
  EXPECT_STREQ("0a", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "12a");
  EXPECT_STREQ("12a", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "123a");
  EXPECT_STREQ("123a", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "1234a");
  EXPECT_STREQ("1234a", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "12345a");
  EXPECT_STREQ("12345a", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "123456a");
  EXPECT_STREQ("123456a", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "1234567A");
  EXPECT_STREQ("1234567A", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "0 a");
  EXPECT_STREQ("0 a", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "12 ab");
  EXPECT_STREQ("12 ab", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "123 a");
  EXPECT_STREQ("123 a", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "1234 a");
  EXPECT_STREQ("1234 a", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "12345 ab");
  EXPECT_STREQ("12,345 ab", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "123456 abc");
  EXPECT_STREQ("123,456 abc", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "1234567 A!=");
  EXPECT_STREQ("1,234,567 A!=", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "0.123");
  EXPECT_STREQ("0.123", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "-0.023");
  EXPECT_STREQ("-0.023", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "-1.2345");
  EXPECT_STREQ("-1.2345", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "-12.34567");
  EXPECT_STREQ("-12.345 67", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "+12.034567");
  EXPECT_STREQ("+12.034 567", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "-123.456789!");
  EXPECT_STREQ("-123.456 789!", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "-1234.5678,");
  EXPECT_STREQ("-1234.5678,", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "$12345.6789?");
  EXPECT_STREQ("$12,345.6789?", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "-$123456.78901.");
  EXPECT_STREQ("-$123,456.789 01.", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "#1234567.8901,A");
  EXPECT_STREQ("#1,234,567.8901,A", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "#1234567.89012345,A");
  EXPECT_STREQ("#1,234,567.890 123 45,A", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "1966-09-24");
  EXPECT_STREQ("1966-09-24", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "January 1, 1901");
  EXPECT_STREQ("January 1, 1901", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "January 1, 1900");
  EXPECT_STREQ("January 1, 1900", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "December 31, 2079");
  EXPECT_STREQ("December 31, 2079", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "December 31, 2080");
  EXPECT_STREQ("December 31, 2080", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "0x");
  EXPECT_STREQ("0x", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "0x0");
  EXPECT_STREQ("0x0", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "0x1f");
  EXPECT_STREQ("0x1f", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "0xfa3");
  EXPECT_STREQ("0xfa3", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "0x4fa3");
  EXPECT_STREQ("0x4fa3", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "0xc4fa3");
  EXPECT_STREQ("0x c 4fa3", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "0XBC4FA3");
  EXPECT_STREQ("0X BC 4FA3", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "0XaBC4FA3");
  EXPECT_STREQ("0X aBC 4FA3", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "0x7abc4fa3");
  EXPECT_STREQ("0x 7abc 4fa3", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "0x07abc4fa3");
  EXPECT_STREQ("0x 0 7abc 4fa3", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "12345e+12");
  EXPECT_STREQ("12,345e+12", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "1.23456e+12");
  EXPECT_STREQ("1.234 56 e+12", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "123456.78901E-123");
  EXPECT_STREQ("123,456.789 01 E-123", _s);
  string_delete(&_s);

  string_new_add_separators(&_s, "-1234567E-308");
  EXPECT_STREQ("-1,234,567E-308", _s);
  string_delete(&_s);
}

TEST(String, RenewAddSeparators) {
  char *_s = null;

  string_new_copy(&_s, "#1234567.8901,A");
  string_renew_add_separators(&_s);
  EXPECT_STREQ("#1,234,567.8901,A", _s);
  string_delete(&_s);

  string_new_copy(&_s, "0x023w3y454h");
  string_renew_add_separators(&_s);
  EXPECT_STREQ("0x023w3y454h", _s);
  string_delete(&_s);
}

TEST(String, RenewTrimLength) {
  char *_s = null;

  string_new_copy(&_s, "1234567");
  string_renew_trim_length(&_s, 7);
  EXPECT_STREQ("1234567", _s);
  _s[6] = '\0';
  string_renew_trim_length(&_s, 7);
  EXPECT_STREQ("123456", _s);
  _s[3] = '\0';
  string_renew_trim_length(&_s, 6);
  EXPECT_STREQ("123", _s);
  _s[0] = '\0';
  string_renew_trim_length(&_s, 3);
  EXPECT_STREQ("", _s);
  string_delete(&_s);
}

TEST(String, RenewAppend) {
  char *_s = null;
  char e[] = "Fair dinkum, mate";

  string_new_copy(&_s, "Fair dinkum");
  string_renew_append(&_s, ", mate");
  EXPECT_STREQ(e, _s);
  string_renew_append(&_s, 0);
  EXPECT_STREQ(e, _s);
  string_delete(&_s);
}

TEST(String, RenewAppendUntilNull) {
  char *_s = null;
  char a[] = "mate";
  char e[] = "Fair dinkum, mate";

  string_new_copy(&_s, "Fair dinkum");
  string_renew_append_until_null(&_s, ", ", "", a, "", null);
  EXPECT_STREQ(e, _s);
  string_renew_append_until_null(&_s, null);
  EXPECT_STREQ(e, _s);
  string_delete(&_s);
}

TEST(String, RenewAppendChar) {
  char e[] = "Fair dinkum!";
  char *_s = null;

  string_new_copy(&_s, "Fair dinkum");
  string_renew_append_char(&_s, '!');
  EXPECT_STREQ(e, _s);
  string_delete(&_s);
}

TEST(String, RenewAppendF) {
  char e[] = "Fair dinkum 42 j 3.1";
  char *_s = null;

  string_new_copy(&_s, "Fair dinkum");
  string_renew_append_f(&_s, " %d %c %.1f", 42, 'j', 3.14159);
  EXPECT_STREQ(e, _s);
  string_delete(&_s);
}

TEST(String, RenewPrepend) {
  char *_s = null;

  string_new_copy(&_s, "");
  string_renew_prepend(&_s, "");
  EXPECT_STREQ("", _s);
  string_renew_prepend(&_s, "a");
  EXPECT_STREQ("a", _s);
  string_renew_prepend(&_s, "");
  EXPECT_STREQ("a", _s);
  string_renew_prepend(&_s, "b");
  EXPECT_STREQ("ba", _s);
  string_renew_prepend(&_s, "");
  EXPECT_STREQ("ba", _s);
  string_renew_prepend(&_s, "c");
  EXPECT_STREQ("cba", _s);
  string_renew_prepend(&_s, "de");
  EXPECT_STREQ("decba", _s);
  string_renew_prepend(&_s, "fgh");
  EXPECT_STREQ("fghdecba", _s);
  string_delete(&_s);
}

TEST(String, RenewPrependF) {
  char *_s = null;

  string_new_copy(&_s, "");
  string_renew_prepend_f(&_s, "");
  EXPECT_STREQ("", _s);
  string_renew_prepend_f(&_s, "%s", "a");
  EXPECT_STREQ("a", _s);
  string_renew_prepend_f(&_s, "");
  EXPECT_STREQ("a", _s);
  string_renew_prepend_f(&_s, "%c", 'b');
  EXPECT_STREQ("ba", _s);
  string_renew_prepend_f(&_s, "");
  EXPECT_STREQ("ba", _s);
  string_renew_prepend_f(&_s, "%sd%s", "c", "e");
  EXPECT_STREQ("cdeba", _s);
  string_delete(&_s);
}

TEST(String, RenewEnsureEndsWith) {
  char *e;
  char *_s = null;

  e = "";
  string_new_copy(&_s, "");
  string_renew_ensure_ends_with(&_s, "", false);
  EXPECT_STREQ(e, _s);
  e = "a";
  string_renew_ensure_ends_with(&_s, "a", false);
  EXPECT_STREQ(e, _s);
  string_renew_ensure_ends_with(&_s, "A", true);
  EXPECT_STREQ(e, _s);
  e = "ab";
  string_renew_ensure_ends_with(&_s, "b", false);
  EXPECT_STREQ(e, _s);
  string_renew_ensure_ends_with(&_s, "b", true);
  EXPECT_STREQ(e, _s);
  string_renew_ensure_ends_with(&_s, "ab", false);
  EXPECT_STREQ(e, _s);
  e = "abba";
  string_renew_ensure_ends_with(&_s, "ba", true);
  EXPECT_STREQ(e, _s);
  string_renew_ensure_ends_with(&_s, "bA", true);
  EXPECT_STREQ(e, _s);
  e = "abbadog";
  string_renew_ensure_ends_with(&_s, "dog", true);
  EXPECT_STREQ(e, _s);
  string_renew_ensure_ends_with(&_s, "DoG", true);
  EXPECT_STREQ(e, _s);
  e = "abbadog/";
  string_renew_ensure_ends_with(&_s, "/", false);
  EXPECT_STREQ(e, _s);
  string_renew_ensure_ends_with(&_s, "/", true);
  EXPECT_STREQ(e, _s);
  string_delete(&_s);
}

TEST(String, RenewCopy) {
  char *_s = null;

  string_new_copy(&_s, "woof");
  string_renew_copy(&_s, "cat");
  string_renew_copy(&_s, "miaow");
  string_renew_copy(&_s, "");
  string_delete(&_s);
}

TEST(String, RenewShorten) {
  char *_s = null;

  string_new_copy(&_s, "dog");
  string_renew_shorten(&_s, 0);
  EXPECT_STREQ("dog", _s);
  string_renew_shorten(&_s, 1);
  EXPECT_STREQ("do", _s);
  string_renew_shorten(&_s, 2);
  EXPECT_STREQ("", _s);
  string_renew_shorten(&_s, 0);
  EXPECT_STREQ("", _s);
  string_delete(&_s);
}

TEST(String, RenewShortenBothEnds) {
  char *_s = null;

  string_new_copy(&_s, "catdog");
  string_renew_shorten_both_ends(&_s, 0);
  EXPECT_STREQ("catdog", _s);
  string_renew_shorten_both_ends(&_s, 1);
  EXPECT_STREQ("atdo", _s);
  string_renew_shorten_both_ends(&_s, 2);
  EXPECT_STREQ("", _s);
  string_renew_shorten_both_ends(&_s, 0);
  EXPECT_STREQ("", _s);
  string_delete(&_s);
}

TEST(String, RenewShortenFromStart) {
  char *_s = null;

  string_new_copy(&_s, "dog");
  string_renew_shorten_from_start(&_s, 0);
  EXPECT_STREQ("dog", _s);
  string_renew_shorten_from_start(&_s, 1);
  EXPECT_STREQ("og", _s);
  string_renew_shorten_from_start(&_s, 2);
  EXPECT_STREQ("", _s);
  string_renew_shorten_from_start(&_s, 0);
  EXPECT_STREQ("", _s);
  string_delete(&_s);
}

TEST(String, RenewIndent) {
  char *_s = null;

  string_new_indent(&_s, "", 0);
  EXPECT_STREQ("", _s);
  string_renew_indent(&_s, 1);
  EXPECT_STREQ(". ", _s);
  string_renew_indent(&_s, 2);
  EXPECT_STREQ(". . . ", _s);
  string_delete(&_s);

  string_new_indent(&_s, "a", 0);
  EXPECT_STREQ("a", _s);
  string_renew_indent(&_s, 1);
  EXPECT_STREQ(". a", _s);
  string_renew_indent(&_s, 2);
  EXPECT_STREQ(". . . a", _s);
  string_delete(&_s);

  string_new_indent(&_s, "ab", 0);
  EXPECT_STREQ("ab", _s);
  string_renew_indent(&_s, 1);
  EXPECT_STREQ(". ab", _s);
  string_renew_indent(&_s, 2);
  EXPECT_STREQ(". . . ab", _s);
  string_delete(&_s);

  string_new_indent(&_s, "\n", 0);
  EXPECT_STREQ("\n", _s);
  string_renew_indent(&_s, 1);
  EXPECT_STREQ(". \n. ", _s);
  string_renew_indent(&_s, 2);
  EXPECT_STREQ(". . . \n. . . ", _s);
  string_delete(&_s);

  string_new_indent(&_s, "a\n", 0);
  EXPECT_STREQ("a\n", _s);
  string_renew_indent(&_s, 1);
  EXPECT_STREQ(". a\n. ", _s);
  string_renew_indent(&_s, 2);
  EXPECT_STREQ(". . . a\n. . . ", _s);
  string_delete(&_s);

  string_new_indent(&_s, "a\nb", 0);
  EXPECT_STREQ("a\nb", _s);
  string_renew_indent(&_s, 1);
  EXPECT_STREQ(". a\n. b", _s);
  string_renew_indent(&_s, 2);
  EXPECT_STREQ(". . . a\n. . . b", _s);
  string_delete(&_s);

  string_new_indent(&_s, "a\nb\nc", 0);
  EXPECT_STREQ("a\nb\nc", _s);
  string_renew_indent(&_s, 1);
  EXPECT_STREQ(". a\n. b\n. c", _s);
  string_renew_indent(&_s, 2);
  EXPECT_STREQ(". . . a\n. . . b\n. . . c", _s);
  string_delete(&_s);
}

TEST(Strings, NewSplit) {
  ulong n;
  char **_splits = null;

  strings_new_split(&_splits, &n, "", '.');
  EXPECT_ULEQ(1, n);
  EXPECT_STREQ("", _splits[0]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, "a", '.');
  EXPECT_ULEQ(1, n);
  EXPECT_STREQ("a", _splits[0]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, "ab", '.');
  EXPECT_ULEQ(1, n);
  EXPECT_STREQ("ab", _splits[0]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, "abc", '.');
  EXPECT_ULEQ(1, n);
  EXPECT_STREQ("abc", _splits[0]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, ".", '.');
  EXPECT_ULEQ(2, n);
  EXPECT_STREQ("", _splits[0]);
  EXPECT_STREQ("", _splits[1]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, "a.", '.');
  EXPECT_ULEQ(2, n);
  EXPECT_STREQ("a", _splits[0]);
  EXPECT_STREQ("", _splits[1]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, ".a", '.');
  EXPECT_ULEQ(2, n);
  EXPECT_STREQ("", _splits[0]);
  EXPECT_STREQ("a", _splits[1]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, "a.b", '.');
  EXPECT_ULEQ(2, n);
  EXPECT_STREQ("a", _splits[0]);
  EXPECT_STREQ("b", _splits[1]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, "ab.", '.');
  EXPECT_ULEQ(2, n);
  EXPECT_STREQ("ab", _splits[0]);
  EXPECT_STREQ("", _splits[1]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, "ab.c", '.');
  EXPECT_ULEQ(2, n);
  EXPECT_STREQ("ab", _splits[0]);
  EXPECT_STREQ("c", _splits[1]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, "ab.cd", '.');
  EXPECT_ULEQ(2, n);
  EXPECT_STREQ("ab", _splits[0]);
  EXPECT_STREQ("cd", _splits[1]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, "a.bc", '.');
  EXPECT_ULEQ(2, n);
  EXPECT_STREQ("a", _splits[0]);
  EXPECT_STREQ("bc", _splits[1]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, ".ab", '.');
  EXPECT_ULEQ(2, n);
  EXPECT_STREQ("", _splits[0]);
  EXPECT_STREQ("ab", _splits[1]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, "..", '.');
  EXPECT_ULEQ(3, n);
  EXPECT_STREQ("", _splits[0]);
  EXPECT_STREQ("", _splits[1]);
  EXPECT_STREQ("", _splits[2]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, "a..", '.');
  EXPECT_ULEQ(3, n);
  EXPECT_STREQ("a", _splits[0]);
  EXPECT_STREQ("", _splits[1]);
  EXPECT_STREQ("", _splits[2]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, ".a.", '.');
  EXPECT_ULEQ(3, n);
  EXPECT_STREQ("", _splits[0]);
  EXPECT_STREQ("a", _splits[1]);
  EXPECT_STREQ("", _splits[2]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, "..a", '.');
  EXPECT_ULEQ(3, n);
  EXPECT_STREQ("", _splits[0]);
  EXPECT_STREQ("", _splits[1]);
  EXPECT_STREQ("a", _splits[2]);
  delete(&_splits, n, string_delete);

  strings_new_split(&_splits, &n, "abc.d.efgh", '.');
  EXPECT_ULEQ(3, n);
  EXPECT_STREQ("abc", _splits[0]);
  EXPECT_STREQ("d", _splits[1]);
  EXPECT_STREQ("efgh", _splits[2]);
  delete(&_splits, n, string_delete);
}

TEST(String, Delete) {
  ushort u;
  ushort v;
  char *_s = null;

  for (u = 1000; u > 0; u--) {
    new(&_s, u);

    for (v = 0; v < u - 1; v++) {
      _s[v] = 'a';
    }

    _s[v] = '\0';
    string_delete(&_s);
  }
}

TEST(String, Print) {
  char *_s = null;

  aprint(&_s, 0, string, "catdog");
  EXPECT_STREQ("[String]\n. Length: 6\n. Contents: 'catdog'\n[/String]\n", _s);
  string_delete(&_s);

  aprint(&_s, 0, string, "cat\tdog\nwoof\f\001\377");
  EXPECT_STREQ(
      "[String]\n. Length: 15\n. String contains nonprintable characters\n"
      ". Nonprintable removed: 'catdogwoof'\n"
      ". Nonprintable replaced by dots: 'cat.dog.woof...'\n"
      ". Escaped: 'cat\\tdog\\nwoof\\f\\001\\377'\n"
      ". Hex: 63 61 74 09 64 6f 67 0a 77 6f 6f 66 0c 01 ff\n"
      "[/String]\n",
      _s);
  string_delete(&_s);
}

TEST(String, IsEmpty) {
  EXPECT_TRUE(string_is_empty(""));
  EXPECT_FALSE(string_is_empty("a"));
  EXPECT_FALSE(string_is_empty("ab"));
  EXPECT_FALSE(string_is_empty("abc"));
}

TEST(String, Length) {
  EXPECT_ULEQ(0, string_length(""));
  EXPECT_ULEQ(1, string_length("a"));
  EXPECT_ULEQ(2, string_length("ab"));
  EXPECT_ULEQ(3, string_length("ab\n"));
  EXPECT_ULEQ(4, string_length("ab\n\xff"));
  EXPECT_ULEQ(5, string_length("ab\n\xffg"));
}

TEST(String, Equals) {
  EXPECT_TRUE(string_equals("", ""));
  EXPECT_TRUE(string_equals("a", "a"));
  EXPECT_TRUE(string_equals("ab", "ab"));
  EXPECT_TRUE(string_equals("abc", "abc"));
  EXPECT_FALSE(string_equals("", "a"));
  EXPECT_FALSE(string_equals("", "ab"));
  EXPECT_FALSE(string_equals("", "abc"));
  EXPECT_FALSE(string_equals("a", ""));
  EXPECT_FALSE(string_equals("a", "ab"));
  EXPECT_FALSE(string_equals("a", "abc"));
  EXPECT_FALSE(string_equals("ab", ""));
  EXPECT_FALSE(string_equals("ab", "a"));
  EXPECT_FALSE(string_equals("ab", "abc"));
  EXPECT_FALSE(string_equals("abc", ""));
  EXPECT_FALSE(string_equals("abc", "a"));
  EXPECT_FALSE(string_equals("abc", "ab"));
}

TEST(String, Compare) {
  char *a;
  char *b;

  a = "";
  b = "";
  EXPECT_LEQ(0, string_compare(a, b, null));

  a = "a";
  b = "a";
  EXPECT_LEQ(0, string_compare(a, b, null));

  a = "ab";
  b = "ab";
  EXPECT_LEQ(0, string_compare(a, b, null));

  a = "";
  b = "a";
  EXPECT_TRUE(string_compare(a, b, null) < 0);
  EXPECT_TRUE(string_compare(b, a, null) > 0);

  a = "a";
  b = "b";
  EXPECT_TRUE(string_compare(a, b, null) < 0);
  EXPECT_TRUE(string_compare(b, a, null) > 0);

  a = "a";
  b = "ab";
  EXPECT_TRUE(string_compare(a, b, null) < 0);
  EXPECT_TRUE(string_compare(b, a, null) > 0);
}

TEST(String, ContainsNonprintable) {
  EXPECT_FALSE(string_contains_nonprintable(""));
  EXPECT_FALSE(string_contains_nonprintable("a"));
  EXPECT_FALSE(string_contains_nonprintable("ab"));
  EXPECT_FALSE(string_contains_nonprintable("A!=~5,./\\"));
  EXPECT_TRUE(string_contains_nonprintable("\t"));
  EXPECT_TRUE(string_contains_nonprintable(" \t"));
  EXPECT_TRUE(string_contains_nonprintable("\t "));
  EXPECT_TRUE(string_contains_nonprintable("\ndog"));
  EXPECT_TRUE(string_contains_nonprintable("cat\f"));
  EXPECT_TRUE(string_contains_nonprintable("ab\xff""c"));
}

TEST(String, ContainsChar) {
  EXPECT_FALSE(string_contains_char("", 'a'));
  EXPECT_FALSE(string_contains_char("b", 'a'));
  EXPECT_FALSE(string_contains_char("bc", 'a'));
  EXPECT_TRUE(string_contains_char("a", 'a'));
  EXPECT_TRUE(string_contains_char("ba", 'a'));
  EXPECT_TRUE(string_contains_char("ab", 'a'));
  EXPECT_TRUE(string_contains_char("bab", 'a'));
  EXPECT_TRUE(string_contains_char("bbab", 'a'));
}

TEST(String, ContainsChars) {
  EXPECT_FALSE(string_contains_chars("", ""));
  EXPECT_FALSE(string_contains_chars("d", ""));
  EXPECT_FALSE(string_contains_chars("de", ""));
  EXPECT_FALSE(string_contains_chars("", "a"));
  EXPECT_FALSE(string_contains_chars("d", "a"));
  EXPECT_FALSE(string_contains_chars("de", "a"));
  EXPECT_FALSE(string_contains_chars("", "ab"));
  EXPECT_FALSE(string_contains_chars("d", "ab"));
  EXPECT_FALSE(string_contains_chars("de", "ab"));
  EXPECT_FALSE(string_contains_chars("", "abc"));
  EXPECT_FALSE(string_contains_chars("d", "abc"));
  EXPECT_FALSE(string_contains_chars("de", "abc"));
  EXPECT_TRUE(string_contains_chars("a", "a"));
  EXPECT_TRUE(string_contains_chars("ab", "a"));
  EXPECT_TRUE(string_contains_chars("ab", "b"));
  EXPECT_TRUE(string_contains_chars("ab", "ab"));
  EXPECT_TRUE(string_contains_chars("ab", "cb"));
  EXPECT_TRUE(string_contains_chars("ab", "ca"));
  EXPECT_TRUE(string_contains_chars("ab", "cda"));
  EXPECT_TRUE(string_contains_chars("ab", "cdea"));
  EXPECT_TRUE(string_contains_chars("ab", "cdeab"));
  EXPECT_TRUE(string_contains_chars("abc", "a"));
  EXPECT_TRUE(string_contains_chars("abc", "defgc"));
  EXPECT_TRUE(string_contains_chars("abc", "defghb"));
}

TEST(String, StartsWith) {
  EXPECT_TRUE(string_starts_with("", "", false));
  EXPECT_TRUE(string_starts_with("", "", true));
  EXPECT_TRUE(string_starts_with("a", "", false));
  EXPECT_TRUE(string_starts_with("a", "", true));
  EXPECT_TRUE(string_starts_with("ab", "", false));
  EXPECT_TRUE(string_starts_with("ab", "", true));
  EXPECT_TRUE(string_starts_with("a", "a", false));
  EXPECT_TRUE(string_starts_with("a", "a", true));
  EXPECT_TRUE(string_starts_with("a", "A", true));
  EXPECT_TRUE(string_starts_with("Ab", "A", false));
  EXPECT_TRUE(string_starts_with("Ab", "A", true));
  EXPECT_TRUE(string_starts_with("Ab", "a", true));
  EXPECT_TRUE(string_starts_with("abc", "a", false));
  EXPECT_TRUE(string_starts_with("abc", "a", true));
  EXPECT_TRUE(string_starts_with("Abc", "a", true));
  EXPECT_TRUE(string_starts_with("ab", "ab", false));
  EXPECT_TRUE(string_starts_with("ab", "ab", true));
  EXPECT_TRUE(string_starts_with("Ab", "ab", true));
  EXPECT_TRUE(string_starts_with("aB", "ab", true));
  EXPECT_TRUE(string_starts_with("AB", "ab", true));
  EXPECT_TRUE(string_starts_with("AB", "AB", true));
  EXPECT_TRUE(string_starts_with("aB", "AB", true));
  EXPECT_TRUE(string_starts_with("abc", "ab", false));
  EXPECT_TRUE(string_starts_with("abc", "ab", true));
  EXPECT_TRUE(string_starts_with("aBc", "ab", true));
  EXPECT_TRUE(string_starts_with("abc", "Ab", true));
  EXPECT_TRUE(string_starts_with("abcd", "ab", false));
  EXPECT_TRUE(string_starts_with("abcd", "AB", true));
  EXPECT_FALSE(string_starts_with("", "a", false));
  EXPECT_FALSE(string_starts_with("", "a", true));
  EXPECT_FALSE(string_starts_with("", "ab", false));
  EXPECT_FALSE(string_starts_with("", "ab", true));
  EXPECT_FALSE(string_starts_with("", "abc", false));
  EXPECT_FALSE(string_starts_with("", "abc", true));
  EXPECT_FALSE(string_starts_with("", "aBc", false));
  EXPECT_FALSE(string_starts_with("a", "ab", false));
  EXPECT_FALSE(string_starts_with("a", "ab", true));
  EXPECT_FALSE(string_starts_with("a", "Ab", true));
  EXPECT_FALSE(string_starts_with("a", "abc", false));
  EXPECT_FALSE(string_starts_with("a", "abc", true));
  EXPECT_FALSE(string_starts_with("ab", "abc", false));
  EXPECT_FALSE(string_starts_with("ab", "abc", true));
  EXPECT_FALSE(string_starts_with("a", "b", false));
  EXPECT_FALSE(string_starts_with("a", "b", true));
  EXPECT_FALSE(string_starts_with("a", "A", false));
  EXPECT_FALSE(string_starts_with("ab", "ba", false));
  EXPECT_FALSE(string_starts_with("ab", "ba", true));
  EXPECT_FALSE(string_starts_with("ab", "Ab", false));
  EXPECT_FALSE(string_starts_with("ab", "aB", false));
  EXPECT_FALSE(string_starts_with("aB", "ab", false));
  EXPECT_FALSE(string_starts_with("Ab", "ab", false));
  EXPECT_FALSE(string_starts_with("abc", "acb", false));
  EXPECT_FALSE(string_starts_with("abc", "acb", true));
}

TEST(String, EndsWith) {
  EXPECT_TRUE(string_ends_with("", "", false));
  EXPECT_TRUE(string_ends_with("", "", true));
  EXPECT_TRUE(string_ends_with("a", "", false));
  EXPECT_TRUE(string_ends_with("a", "", true));
  EXPECT_TRUE(string_ends_with("ab", "", false));
  EXPECT_TRUE(string_ends_with("ab", "", true));
  EXPECT_TRUE(string_ends_with("a", "a", false));
  EXPECT_TRUE(string_ends_with("a", "a", true));
  EXPECT_TRUE(string_ends_with("a", "A", true));
  EXPECT_TRUE(string_ends_with("ba", "a", false));
  EXPECT_TRUE(string_ends_with("ba", "a", true));
  EXPECT_TRUE(string_ends_with("bA", "a", true));
  EXPECT_TRUE(string_ends_with("cba", "a", false));
  EXPECT_TRUE(string_ends_with("cba", "a", true));
  EXPECT_TRUE(string_ends_with("cba", "A", true));
  EXPECT_TRUE(string_ends_with("ba", "ba", false));
  EXPECT_TRUE(string_ends_with("ba", "ba", true));
  EXPECT_TRUE(string_ends_with("ba", "ba", true));
  EXPECT_TRUE(string_ends_with("Ba", "ba", true));
  EXPECT_TRUE(string_ends_with("bA", "ba", true));
  EXPECT_TRUE(string_ends_with("ba", "Ba", true));
  EXPECT_TRUE(string_ends_with("ba", "bA", true));
  EXPECT_TRUE(string_ends_with("Ba", "bA", true));
  EXPECT_TRUE(string_ends_with("cba", "ba", false));
  EXPECT_TRUE(string_ends_with("cba", "ba", true));
  EXPECT_TRUE(string_ends_with("cBa", "bA", true));
  EXPECT_TRUE(string_ends_with("dcba", "ba", false));
  EXPECT_TRUE(string_ends_with("dcba", "ba", true));
  EXPECT_TRUE(string_ends_with("dcbA", "Ba", true));
  EXPECT_FALSE(string_ends_with("", "a", false));
  EXPECT_FALSE(string_ends_with("", "a", true));
  EXPECT_FALSE(string_ends_with("", "ba", false));
  EXPECT_FALSE(string_ends_with("", "ba", true));
  EXPECT_FALSE(string_ends_with("", "cba", false));
  EXPECT_FALSE(string_ends_with("", "cba", true));
  EXPECT_FALSE(string_ends_with("a", "ba", false));
  EXPECT_FALSE(string_ends_with("a", "ba", true));
  EXPECT_FALSE(string_ends_with("ba", "A", false));
  EXPECT_FALSE(string_ends_with("a", "cba", false));
  EXPECT_FALSE(string_ends_with("a", "cba", true));
  EXPECT_FALSE(string_ends_with("ba", "cba", false));
  EXPECT_FALSE(string_ends_with("ba", "cba", true));
  EXPECT_FALSE(string_ends_with("cBa", "ba", false));
  EXPECT_FALSE(string_ends_with("cba", "Ba", false));
  EXPECT_FALSE(string_ends_with("cBa", "bA", false));
  EXPECT_FALSE(string_starts_with("a", "b", false));
  EXPECT_FALSE(string_starts_with("a", "b", true));
  EXPECT_FALSE(string_starts_with("a", "b", false));
  EXPECT_FALSE(string_starts_with("A", "b", true));
  EXPECT_FALSE(string_starts_with("A", "b", false));
  EXPECT_FALSE(string_starts_with("a", "B", true));
  EXPECT_FALSE(string_starts_with("a", "B", true));
  EXPECT_FALSE(string_starts_with("ba", "ab", false));
  EXPECT_FALSE(string_starts_with("ba", "ab", true))
  EXPECT_FALSE(string_starts_with("cba", "cab", false));
  EXPECT_FALSE(string_starts_with("cba", "cab", true));
}

TEST(String, FirstIndexOf) {
  /* Simple cases.
   */
  EXPECT_ULEQ(-1, string_first_index_of("", 'a'));
  EXPECT_ULEQ(0, string_first_index_of("a", 'a'));
  EXPECT_ULEQ(-1, string_first_index_of("a", 'b'));
  EXPECT_ULEQ(0, string_first_index_of("as", 'a'));
  EXPECT_ULEQ(1, string_first_index_of("as", 's'));
  EXPECT_ULEQ(-1, string_first_index_of("as", 'b'));
  EXPECT_ULEQ(0, string_first_index_of("dog", 'd'));
  EXPECT_ULEQ(1, string_first_index_of("dog", 'o'));
  EXPECT_ULEQ(2, string_first_index_of("dog", 'g'));
  EXPECT_ULEQ(-1, string_first_index_of("dog", 'b'));

  /* More testing cases.
   */
  EXPECT_ULEQ(0, string_first_index_of("dad", 'd'));
  EXPECT_ULEQ(1, string_first_index_of("appear", 'p'));
  EXPECT_ULEQ(2, string_first_index_of("batt", 't'));
  EXPECT_ULEQ(1, string_first_index_of("Mississippi", 'i'));
}

TEST(String, NthIndexOf) {
  EXPECT_ULEQ(-1, string_nth_index_of("", 0, 'a'));
  EXPECT_ULEQ(-1, string_nth_index_of("", 1, 'a'));
  EXPECT_ULEQ(-1, string_nth_index_of("", 2, 'a'));
  EXPECT_ULEQ(0, string_nth_index_of("a", 0, 'a'));
  EXPECT_ULEQ(-1, string_nth_index_of("a", 1, 'a'));
  EXPECT_ULEQ(-1, string_nth_index_of("a", 2, 'a'));
  EXPECT_ULEQ(-1, string_nth_index_of("a", 0, 'b'));
  EXPECT_ULEQ(-1, string_nth_index_of("a", 1, 'b'));
  EXPECT_ULEQ(-1, string_nth_index_of("a", 2, 'b'));
  EXPECT_ULEQ(0, string_nth_index_of("as", 0, 'a'));
  EXPECT_ULEQ(-1, string_nth_index_of("as", 1, 'a'));
  EXPECT_ULEQ(-1, string_nth_index_of("as", 2, 'a'));
  EXPECT_ULEQ(1, string_nth_index_of("as", 0, 's'));
  EXPECT_ULEQ(-1, string_nth_index_of("as", 1, 's'));
  EXPECT_ULEQ(-1, string_nth_index_of("as", 2, 's'));
  EXPECT_ULEQ(0, string_nth_index_of("dad", 0, 'd'));
  EXPECT_ULEQ(2, string_nth_index_of("dad", 1, 'd'));
  EXPECT_ULEQ(-1, string_nth_index_of("dad", 2, 'd'));
  EXPECT_ULEQ(1, string_nth_index_of("appear", 0, 'p'));
  EXPECT_ULEQ(2, string_nth_index_of("appear", 1, 'p'));
  EXPECT_ULEQ(-1, string_nth_index_of("appear", 2, 'p'));
  EXPECT_ULEQ(2, string_nth_index_of("batt", 0, 't'));
  EXPECT_ULEQ(3, string_nth_index_of("batt", 1, 't'));
  EXPECT_ULEQ(1, string_nth_index_of("Mississippi", 0, 'i'));
  EXPECT_ULEQ(4, string_nth_index_of("Mississippi", 1, 'i'));
  EXPECT_ULEQ(7, string_nth_index_of("Mississippi", 2, 'i'));
  EXPECT_ULEQ(10, string_nth_index_of("Mississippi", 3, 'i'));
  EXPECT_ULEQ(-1, string_nth_index_of("Mississippi", 4, 'i'));
}

TEST(String, LastIndexOf) {
  /* Simple cases.
   */
  EXPECT_ULEQ(-1, string_last_index_of("", 'a'));
  EXPECT_ULEQ(0, string_last_index_of("a", 'a'));
  EXPECT_ULEQ(-1, string_last_index_of("a", 'b'));
  EXPECT_ULEQ(0, string_last_index_of("as", 'a'));
  EXPECT_ULEQ(1, string_last_index_of("as", 's'));
  EXPECT_ULEQ(-1, string_last_index_of("as", 'b'));
  EXPECT_ULEQ(0, string_last_index_of("dog", 'd'));
  EXPECT_ULEQ(1, string_last_index_of("dog", 'o'));
  EXPECT_ULEQ(2, string_last_index_of("dog", 'g'));
  EXPECT_ULEQ(-1, string_last_index_of("dog", 'b'));

  /* More testing cases.
   */
  EXPECT_ULEQ(2, string_last_index_of("dad", 'd'));
  EXPECT_ULEQ(2, string_last_index_of("appear", 'p'));
  EXPECT_ULEQ(3, string_last_index_of("batt", 't'));
  EXPECT_ULEQ(10, string_last_index_of("Mississippi", 'i'));
}

TEST(String, NthLastIndexOf) {
  EXPECT_ULEQ(-1, string_nth_last_index_of("", 0, 'a'));
  EXPECT_ULEQ(-1, string_nth_last_index_of("", 1, 'a'));
  EXPECT_ULEQ(-1, string_nth_last_index_of("", 2, 'a'));
  EXPECT_ULEQ(0, string_nth_last_index_of("a", 0, 'a'));
  EXPECT_ULEQ(-1, string_nth_last_index_of("a", 1, 'a'));
  EXPECT_ULEQ(-1, string_nth_last_index_of("a", 2, 'a'));
  EXPECT_ULEQ(-1, string_nth_last_index_of("a", 0, 'b'));
  EXPECT_ULEQ(-1, string_nth_last_index_of("a", 1, 'b'));
  EXPECT_ULEQ(-1, string_nth_last_index_of("a", 2, 'b'));
  EXPECT_ULEQ(0, string_nth_last_index_of("as", 0, 'a'));
  EXPECT_ULEQ(-1, string_nth_last_index_of("as", 1, 'a'));
  EXPECT_ULEQ(-1, string_nth_last_index_of("as", 2, 'a'));
  EXPECT_ULEQ(1, string_nth_last_index_of("as", 0, 's'));
  EXPECT_ULEQ(-1, string_nth_last_index_of("as", 1, 's'));
  EXPECT_ULEQ(-1, string_nth_last_index_of("as", 2, 's'));
  EXPECT_ULEQ(2, string_nth_last_index_of("dad", 0, 'd'));
  EXPECT_ULEQ(0, string_nth_last_index_of("dad", 1, 'd'));
  EXPECT_ULEQ(-1, string_nth_last_index_of("dad", 2, 'd'));
  EXPECT_ULEQ(2, string_nth_last_index_of("appear", 0, 'p'));
  EXPECT_ULEQ(1, string_nth_last_index_of("appear", 1, 'p'));
  EXPECT_ULEQ(-1, string_nth_last_index_of("appear", 2, 'p'));
  EXPECT_ULEQ(3, string_nth_last_index_of("batt", 0, 't'));
  EXPECT_ULEQ(2, string_nth_last_index_of("batt", 1, 't'));
  EXPECT_ULEQ(10, string_nth_last_index_of("Mississippi", 0, 'i'));
  EXPECT_ULEQ(7, string_nth_last_index_of("Mississippi", 1, 'i'));
  EXPECT_ULEQ(4, string_nth_last_index_of("Mississippi", 2, 'i'));
  EXPECT_ULEQ(1, string_nth_last_index_of("Mississippi", 3, 'i'));
  EXPECT_ULEQ(-1, string_nth_last_index_of("Mississippi", 4, 'i'));
}

TEST(String, NumOccurrences) {
  EXPECT_ULEQ(0, string_num_occurrences("", 'a'));
  EXPECT_ULEQ(0, string_num_occurrences("b", 'a'));
  EXPECT_ULEQ(0, string_num_occurrences("A", 'a'));
  EXPECT_ULEQ(0, string_num_occurrences("cb", 'a'));
  EXPECT_ULEQ(0, string_num_occurrences("def", 'a'));

  EXPECT_ULEQ(1, string_num_occurrences("a", 'a'));
  EXPECT_ULEQ(1, string_num_occurrences("ba", 'a'));
  EXPECT_ULEQ(1, string_num_occurrences("ab", 'a'));
  EXPECT_ULEQ(1, string_num_occurrences("bab", 'a'));
  EXPECT_ULEQ(1, string_num_occurrences("cba", 'a'));
  EXPECT_ULEQ(1, string_num_occurrences("abc", 'a'));

  EXPECT_ULEQ(2, string_num_occurrences("aa", 'a'));
  EXPECT_ULEQ(2, string_num_occurrences("baa", 'a'));
  EXPECT_ULEQ(2, string_num_occurrences("aba", 'a'));
  EXPECT_ULEQ(2, string_num_occurrences("aab", 'a'));
  EXPECT_ULEQ(2, string_num_occurrences("baba", 'a'));
  EXPECT_ULEQ(2, string_num_occurrences("abab", 'a'));
  EXPECT_ULEQ(2, string_num_occurrences("cbabhayt", 'a'));
  EXPECT_ULEQ(2, string_num_occurrences("aaggh", 'a'));
}

TEST(String, SubstringEquals) {
  EXPECT_TRUE(string_substring_equals("", 0, 0, ""));
  EXPECT_TRUE(string_substring_equals("a", 0, 0, ""));
  EXPECT_TRUE(string_substring_equals("ab", 0, 0, ""));
  EXPECT_TRUE(string_substring_equals("", 0, 1, ""));
  EXPECT_TRUE(string_substring_equals("", 0, 10, ""));
  EXPECT_TRUE(string_substring_equals("a", 0, 1, "a"));
  EXPECT_TRUE(string_substring_equals("ab", 0, 1, "a"));
  EXPECT_TRUE(string_substring_equals("abc", 0, 1, "a"));
  EXPECT_TRUE(string_substring_equals("ab", 1, 2, "b"));
  EXPECT_TRUE(string_substring_equals("abc", 1, 2, "b"));
  EXPECT_TRUE(string_substring_equals("abc", 1, 3, "bc"));
  EXPECT_TRUE(string_substring_equals("abc", 1, 10, "bc"));
  EXPECT_FALSE(string_substring_equals("a", 0, 1, "A"));
  EXPECT_FALSE(string_substring_equals("ab", 0, 1, "A"));
  EXPECT_FALSE(string_substring_equals("abc", 0, 1, "A"));
  EXPECT_FALSE(string_substring_equals("ab", 1, 2, "B"));
  EXPECT_FALSE(string_substring_equals("abc", 1, 2, "B"));
  EXPECT_FALSE(string_substring_equals("abc", 1, 3, "bC"));
  EXPECT_FALSE(string_substring_equals("abc", 1, 10, "Bc"));
  EXPECT_TRUE(string_substring_equals("abcde", 2, 4, "cd"));
  EXPECT_FALSE(string_substring_equals("abcde", 2, 4, "c"));
}

TEST(String, ContainsAt) {
  /* Contains an empty string anywhere, as long as it's within the string.
   */
  EXPECT_TRUE(string_contains_at("", 0, ""));
  EXPECT_FALSE(string_contains_at("", 1, ""));
  EXPECT_TRUE(string_contains_at("a", 0, ""));
  EXPECT_TRUE(string_contains_at("a", 1, ""));
  EXPECT_FALSE(string_contains_at("a", 2, ""));
  EXPECT_TRUE(string_contains_at("ab", 0, ""));
  EXPECT_TRUE(string_contains_at("ab", 1, ""));
  EXPECT_TRUE(string_contains_at("ab", 2, ""));
  EXPECT_FALSE(string_contains_at("ab", 3, ""));

  /* Other is length 1.
   */
  EXPECT_FALSE(string_contains_at("", 0, "a"));
  EXPECT_FALSE(string_contains_at("b", 0, "a"));
  EXPECT_FALSE(string_contains_at("ba", 0, "a"));
  EXPECT_FALSE(string_contains_at("baa", 0, "a"));
  EXPECT_TRUE(string_contains_at("a", 0, "a"));
  EXPECT_TRUE(string_contains_at("ab", 0, "a"));
  EXPECT_TRUE(string_contains_at("abb", 0, "a"));
  EXPECT_FALSE(string_contains_at("", 1, "a"));
  EXPECT_FALSE(string_contains_at("a", 1, "a"));
  EXPECT_FALSE(string_contains_at("ab", 1, "a"));
  EXPECT_FALSE(string_contains_at("aba", 1, "a"));
  EXPECT_FALSE(string_contains_at("abaa", 1, "a"));
  EXPECT_TRUE(string_contains_at("ba", 1, "a"));
  EXPECT_TRUE(string_contains_at("bab", 1, "a"));
  EXPECT_TRUE(string_contains_at("babb", 1, "a"));
  EXPECT_FALSE(string_contains_at("", 2, "a"));
  EXPECT_FALSE(string_contains_at("a", 2, "a"));
  EXPECT_FALSE(string_contains_at("aa", 2, "a"));
  EXPECT_FALSE(string_contains_at("aab", 2, "a"));
  EXPECT_FALSE(string_contains_at("aaba", 2, "a"));
  EXPECT_FALSE(string_contains_at("aabaa", 2, "a"));
  EXPECT_TRUE(string_contains_at("bba", 2, "a"));
  EXPECT_TRUE(string_contains_at("bbab", 2, "a"));
  EXPECT_TRUE(string_contains_at("bbabb", 2, "a"));

  /* Other is length 2.
   */
  EXPECT_FALSE(string_contains_at("", 0, "ab"));
  EXPECT_FALSE(string_contains_at("a", 0, "ab"));
  EXPECT_FALSE(string_contains_at("bb", 0, "ab"));
  EXPECT_FALSE(string_contains_at("ba", 0, "ab"));
  EXPECT_FALSE(string_contains_at("aab", 0, "ab"));
  EXPECT_TRUE(string_contains_at("ab", 0, "ab"));
  EXPECT_TRUE(string_contains_at("aba", 0, "ab"));
  EXPECT_TRUE(string_contains_at("abb", 0, "ab"));
  EXPECT_FALSE(string_contains_at("", 1, "ab"));
  EXPECT_FALSE(string_contains_at("a", 1, "ab"));
  EXPECT_FALSE(string_contains_at("ab", 1, "ab"));
  EXPECT_FALSE(string_contains_at("abb", 1, "ab"));
  EXPECT_TRUE(string_contains_at("aab", 1, "ab"));
  EXPECT_TRUE(string_contains_at("aaba", 1, "ab"));
  EXPECT_TRUE(string_contains_at("aabab", 1, "ab"));
}

TEST(String, ReplaceChars) {
  char *_s = null;

  string_new_copy(&_s, "a:bad/set of\\characters\n");
  string_replace_chars(_s, " :/\\\n", '|');
  EXPECT_STREQ("a|bad|set|of|characters|", _s);
  string_delete(&_s);
}

TEST(String, TrimTrailingWhitespace) {
  char *_s = null;

  string_new_copy(&_s, "");
  string_trim_trailing_whitespace(_s);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_copy(&_s, "a");
  string_trim_trailing_whitespace(_s);
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_copy(&_s, " ");
  string_trim_trailing_whitespace(_s);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_copy(&_s, " a");
  string_trim_trailing_whitespace(_s);
  EXPECT_STREQ(" a", _s);
  string_delete(&_s);

  string_new_copy(&_s, "a ");
  string_trim_trailing_whitespace(_s);
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  string_new_copy(&_s, "aa");
  string_trim_trailing_whitespace(_s);
  EXPECT_STREQ("aa", _s);
  string_delete(&_s);

  string_new_copy(&_s, "  ");
  string_trim_trailing_whitespace(_s);
  EXPECT_STREQ("", _s);
  string_delete(&_s);

  string_new_copy(&_s, "  a");
  string_trim_trailing_whitespace(_s);
  EXPECT_STREQ("  a", _s);
  string_delete(&_s);

  string_new_copy(&_s, " a ");
  string_trim_trailing_whitespace(_s);
  EXPECT_STREQ(" a", _s);
  string_delete(&_s);

  string_new_copy(&_s, "a  ");
  string_trim_trailing_whitespace(_s);
  EXPECT_STREQ("a", _s);
  string_delete(&_s);
}

TEST(String, NewCopyWrap) {
  bool w;
  char *orig;
  char *_s = null;

  orig = "This is a long string with alongwordinitman";

  string_new_copy_wrap(&_s, orig, 0, 80, 0, &w);
  EXPECT_FALSE(w);
  EXPECT_STREQ(orig, _s);
  string_delete(&_s);

  string_new_copy_wrap(&_s, orig, 0, 10, 0, &w);
  EXPECT_TRUE(w);
  EXPECT_STREQ("This is a\nlong\nstring\nwith\nalongwordi\nnitman", _s);
  string_delete(&_s);

  string_new_copy_wrap(&_s, orig, 0, 5, 0, &w);
  EXPECT_TRUE(w);
  EXPECT_STREQ("This\nis a\nlong\nstrin\ng\nwith\nalong\nwordi\nnitma\nn", _s);
  string_delete(&_s);

  string_new_copy_wrap(&_s, orig, 0, 4, 0, &w);
  EXPECT_TRUE(w);
  EXPECT_STREQ("This\nis a\nlong\nstri\nng\nwith\nalon\ngwor\ndini\ntman", _s);
  string_delete(&_s);

  string_new_copy_wrap(&_s, orig, 60, 80, 0, &w);
  EXPECT_TRUE(w);
  EXPECT_STREQ("This is a long\nstring with alongwordinitman", _s);
  string_delete(&_s);

  string_new_copy_wrap(&_s, orig, 59, 80, 0, &w);
  EXPECT_TRUE(w);
  EXPECT_STREQ("This is a long string\nwith alongwordinitman", _s);
  string_delete(&_s);

  string_new_copy_wrap(&_s, orig, 0, 10, 1, &w);
  EXPECT_TRUE(w);
  EXPECT_STREQ("This is a\n long\n string\n with\n alongword\n initman", _s);
  string_delete(&_s);

  string_new_copy_wrap(&_s, "More of an indent", 0, 4, 2, &w);
  EXPECT_TRUE(w);
  EXPECT_STREQ("More\n  of\n  an\n  in\n  de\n  nt", _s);
  string_delete(&_s);

  string_new_copy_wrap(&_s, "More of an indent", 1, 4, 2, &w);
  EXPECT_TRUE(w);
  EXPECT_STREQ("Mor\n  e\n  of\n  an\n  in\n  de\n  nt", _s);
  string_delete(&_s);

  string_new_copy_wrap(&_s, "More of a gap", 2, 4, 3, &w);
  EXPECT_TRUE(w);
  EXPECT_STREQ("Mo\n   r\n   e\n   o\n   f\n   a\n   g\n   a\n   p", _s);
  string_delete(&_s);

  string_new_copy_wrap(
      &_s,
      "Let's use    some    large  gaps  here",
      0,
      10,
      0,
      &w);
  EXPECT_TRUE(w);
  EXPECT_STREQ("Let's use\nsome\nlarge\ngaps  here", _s);
  string_delete(&_s);

  string_new_copy_wrap(&_s, "Space-end  ", 0, 10, 0, &w);
  EXPECT_FALSE(w);
  EXPECT_STREQ("Space-end", _s);
  string_delete(&_s);

  string_new_copy_wrap(&_s, "Space    end            ", 0, 5, 0, &w);
  EXPECT_TRUE(w);
  EXPECT_STREQ("Space\nend", _s);
  string_delete(&_s);
}

TEST(String, RenewWrap) {
  bool w;
  char *_s = null;

  string_new_copy(&_s, "More of an indent");
  string_renew_wrap(&_s, 1, 4, 2, &w);
  EXPECT_TRUE(w);
  EXPECT_STREQ("Mor\n  e\n  of\n  an\n  in\n  de\n  nt", _s);
  string_delete(&_s);
}

TEST(String, RenewAppendWrap) {
  bool w;
  char *_s = null;

  string_new_f_wrap(&_s, 0, 6, 2, &w, "This is a great %s", "one");
  EXPECT_TRUE(w);
  string_renew_append(&_s, "\n\n");
  string_renew_append_wrap(
      &_s,
      "As is this",
      0,
      5,
      1,
      &w);
  EXPECT_TRUE(w);
  EXPECT_STREQ("This\n  is a\n  grea\n  t\n  one\n\nAs is\n this", _s);
  string_delete(&_s);
}

TEST(String, NewFWrap) {
  bool w;
  char *_s = null;

  string_new_f_wrap(&_s, 1, 4, 2, &w, "More %s %d indents    ", "of", 27);
  EXPECT_TRUE(w);
  EXPECT_STREQ("Mor\n  e\n  of\n  27\n  in\n  de\n  nt\n  s", _s);
  string_delete(&_s);
}

TEST(String, RenewAppendFWrap) {
  bool w;
  char *_s = null;

  string_new_f_wrap(&_s, 0, 6, 2, &w, "This is a great %s", "one");
  EXPECT_TRUE(w);
  string_renew_append(&_s, "\n\n");
  string_renew_append_f_wrap(
      &_s,
      0,
      5,
      1,
      &w,
      "As is this %d for us %d do",
      1,
      2);
  EXPECT_TRUE(w);
  EXPECT_STREQ(
      "This\n  is a\n  grea\n  t\n  one\n\nAs is\n this\n 1\n for\n us 2\n do",
      _s);
  string_delete(&_s);
}

TEST(String, Copy) {
  char s[10];
  char *e = "";

  string_copy(e, s, 0);
  EXPECT_STREQ(e, s);
  string_copy(e, s, 1);
  EXPECT_STREQ(e, s);
  e = "a";
  string_copy(e, s, 1);
  EXPECT_STREQ(e, s);
  string_copy(e, s, 2);
  EXPECT_STREQ(e, s);
  e = "ab";
  string_copy(e, s, 2);
  EXPECT_STREQ(e, s);
  string_copy(e, s, 3);
  EXPECT_STREQ(e, s);
  e = "123456789";
  string_copy(e, s, 9);
  EXPECT_STREQ(e, s);
  string_copy(e, s, 9999);
  EXPECT_STREQ(e, s);
}

TEST(String, FGetMaxLen) {
  int n;

  /* Empty.
   */
  EXPECT_ULEQ(0, string_f_get_max_len(""));

  /* Simple characters.
   */
  EXPECT_ULEQ(1, string_f_get_max_len("a"));
  EXPECT_ULEQ(2, string_f_get_max_len("ad"));
  EXPECT_ULEQ(3, string_f_get_max_len("ald"));

  /* %.
   */
  EXPECT_ULEQ(1, string_f_get_max_len("%%"));
  EXPECT_ULEQ(2, string_f_get_max_len("%%%%"));
  EXPECT_ULEQ(2, string_f_get_max_len("a%%"));
  EXPECT_ULEQ(2, string_f_get_max_len("%%f"));
  EXPECT_ULEQ(3, string_f_get_max_len("%%%%%%"));
  EXPECT_ULEQ(3, string_f_get_max_len("%%d%%"));
  EXPECT_ULEQ(3, string_f_get_max_len("d%%%%"));
  EXPECT_ULEQ(3, string_f_get_max_len("%%%%d"));

  /* %n.
   */
  EXPECT_ULEQ(0, string_f_get_max_len("%n"));
  EXPECT_ULEQ(0, string_f_get_max_len("%n%n"));
  EXPECT_ULEQ(1, string_f_get_max_len("%na%n"));

  /* Specifying minimum width.
   */
  EXPECT_ULEQ(17, string_f_get_max_len("%17d", 0));
  EXPECT_ULEQ(17, string_f_get_max_len("%17i", 0));
  EXPECT_ULEQ(99, string_f_get_max_len("%99c", 'x'));
  EXPECT_ULEQ(123, string_f_get_max_len("%123x", 0));
  EXPECT_ULEQ(123, string_f_get_max_len("%123X", 0));
  EXPECT_ULEQ(123, string_f_get_max_len("%123o", 0));
  EXPECT_ULEQ(1234, string_f_get_max_len("%1234u", 0));
  EXPECT_ULEQ(2, string_f_get_max_len("%2s", ""));
  EXPECT_ULEQ(15, string_f_get_max_len("%15f", 0));
  EXPECT_ULEQ(15, string_f_get_max_len("%15e", 0));
  EXPECT_ULEQ(15, string_f_get_max_len("%15E", 0));
  EXPECT_ULEQ(15, string_f_get_max_len("%15g", 0));
  EXPECT_ULEQ(15, string_f_get_max_len("%15G", 0));
  EXPECT_ULEQ(40, string_f_get_max_len("%40p", null));
  EXPECT_ULEQ(0, string_f_get_max_len("%20n", &n));

  /* Check defaults.
   */
  EXPECT_ULEQ(6, string_f_get_max_len("%hd", 0));
  EXPECT_ULEQ(11, string_f_get_max_len("%d", 0));
  EXPECT_ULEQ(21, string_f_get_max_len("%ld", 0));
  EXPECT_ULEQ(6, string_f_get_max_len("%hi", 0));
  EXPECT_ULEQ(11, string_f_get_max_len("%i", 0));
  EXPECT_ULEQ(21, string_f_get_max_len("%li", 0));
  EXPECT_ULEQ(6, string_f_get_max_len("%hu", 0));
  EXPECT_ULEQ(11, string_f_get_max_len("%u", 0));
  EXPECT_ULEQ(21, string_f_get_max_len("%lu", 0));
  EXPECT_ULEQ(7, string_f_get_max_len("%ho", 0));
  EXPECT_ULEQ(12, string_f_get_max_len("%o", 0));
  EXPECT_ULEQ(23, string_f_get_max_len("%lo", 0));
  EXPECT_ULEQ(8, string_f_get_max_len("%#ho", 0));
  EXPECT_ULEQ(13, string_f_get_max_len("%#o", 0));
  EXPECT_ULEQ(24, string_f_get_max_len("%#lo", 0));
  EXPECT_ULEQ(4, string_f_get_max_len("%hx", 0));
  EXPECT_ULEQ(8, string_f_get_max_len("%x", 0));
  EXPECT_ULEQ(16, string_f_get_max_len("%lx", 0));
  EXPECT_ULEQ(16, string_f_get_max_len("%lX", 0));
  EXPECT_ULEQ(6, string_f_get_max_len("%#hx", 0));
  EXPECT_ULEQ(10, string_f_get_max_len("%#x", 0));
  EXPECT_ULEQ(18, string_f_get_max_len("%#lx", 0));
  EXPECT_ULEQ(18, string_f_get_max_len("%#lX", 0));
  EXPECT_ULEQ(1, string_f_get_max_len("%c", 0));
  EXPECT_ULEQ(5, string_f_get_max_len("%s", "bingo"));
  EXPECT_ULEQ(9, string_f_get_max_len("%f", 0.0));
  EXPECT_ULEQ(9, string_f_get_max_len("%f", 0.1));
  EXPECT_ULEQ(9, string_f_get_max_len("%f", 1.0));
  EXPECT_ULEQ(9, string_f_get_max_len("%f", 9.999));
  EXPECT_ULEQ(10, string_f_get_max_len("%f", 10.0));
  EXPECT_ULEQ(10, string_f_get_max_len("%f", 99.9));
  EXPECT_ULEQ(10, string_f_get_max_len("%f", -99.9));
  EXPECT_ULEQ(11, string_f_get_max_len("%f", 100.0));
  EXPECT_ULEQ(11, string_f_get_max_len("%f", -999.9));
  EXPECT_ULEQ(309, string_f_get_max_len("%f", 1e300));
  EXPECT_ULEQ(9, string_f_get_max_len("%Lf", 0.0L));
  EXPECT_ULEQ(9, string_f_get_max_len("%Lf", 0.1L));
  EXPECT_ULEQ(9, string_f_get_max_len("%Lf", 1.0L));
  EXPECT_ULEQ(9, string_f_get_max_len("%Lf", 9.999L));
  EXPECT_ULEQ(10, string_f_get_max_len("%Lf", 10.0L));
  EXPECT_ULEQ(10, string_f_get_max_len("%Lf", 99.9L));
  EXPECT_ULEQ(10, string_f_get_max_len("%Lf", -99.9L));
  EXPECT_ULEQ(11, string_f_get_max_len("%Lf", 100.0L));
  EXPECT_ULEQ(11, string_f_get_max_len("%Lf", -999.9L));
  EXPECT_ULEQ(309, string_f_get_max_len("%Lf", 1e300L));
  EXPECT_ULEQ(15, string_f_get_max_len("%e", 0.0));
  EXPECT_ULEQ(15, string_f_get_max_len("%E", 0.0));
  EXPECT_ULEQ(15, string_f_get_max_len("%Le", 0.0L));
  EXPECT_ULEQ(15, string_f_get_max_len("%LE", 0.0L));
  EXPECT_ULEQ(9, string_f_get_max_len("%g", 0.0));
  EXPECT_ULEQ(9, string_f_get_max_len("%g", 0.1));
  EXPECT_ULEQ(10, string_f_get_max_len("%g", 0.01));
  EXPECT_ULEQ(11, string_f_get_max_len("%g", 0.001));
  EXPECT_ULEQ(15, string_f_get_max_len("%g", 0.0001));
  EXPECT_ULEQ(16, string_f_get_max_len("%g", -0.0000999));
  EXPECT_ULEQ(15, string_f_get_max_len("%g", 0.00001));
  EXPECT_ULEQ(15, string_f_get_max_len("%g", 1234567.0));
  EXPECT_ULEQ(15, string_f_get_max_len("%g", -123456.0));
  EXPECT_ULEQ(13, string_f_get_max_len("%g", 12345.0));
  EXPECT_ULEQ(12, string_f_get_max_len("%g", 1234.0));
  EXPECT_ULEQ(15, string_f_get_max_len("%g", 12345678.0));
  EXPECT_ULEQ(15, string_f_get_max_len("%g", 123456789.0));
  EXPECT_ULEQ(32, string_f_get_max_len("%p", null));

  /* Set precision.
   */
  EXPECT_ULEQ(1, string_f_get_max_len("%.1s", "doggie"));
  EXPECT_ULEQ(2, string_f_get_max_len("%.2s", "doggie"));
  EXPECT_ULEQ(6, string_f_get_max_len("%.9s", "doggie"));
  EXPECT_ULEQ(12, string_f_get_max_len("%.9f", 1.4));
  EXPECT_ULEQ(3, string_f_get_max_len("%.0f", 1.4));
  EXPECT_ULEQ(13, string_f_get_max_len("%.10f", 1.4));
  EXPECT_ULEQ(14, string_f_get_max_len("%.10f", -19.4));

  /* *.
   */
  EXPECT_ULEQ(6, string_f_get_max_len("%*s", 3, "doggie"));
  EXPECT_ULEQ(10, string_f_get_max_len("%*s", 10, "doggie"));
  EXPECT_ULEQ(3, string_f_get_max_len("%.*s", 3, "doggie"));
  EXPECT_ULEQ(6, string_f_get_max_len("%.*s", 10, "doggie"));
}

FINISH
