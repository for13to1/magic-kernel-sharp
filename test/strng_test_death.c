#include "strng_test_death.h"

START_DEATH

TEST(String, NewEmpty) {
  char *_s = null;

  string_new_empty(&_s);
  EXPECT_DEATH(string_new_empty(&_s), "Pointer not empty");
  string_delete(&_s);
}

TEST(String, NewChar) {
  char *_s = null;

  string_new_char(&_s, 'p');
  EXPECT_DEATH(string_new_char(&_s, 'w'), "Pointer not empty");
  string_delete(&_s);
}

TEST(String, NewCopy) {
  char orig[] = "Hi there";
  char *_copy = null;
  char *local;

  string_new_copy(&_copy, orig);
  local = _copy;
  string_delete(&_copy);
  EXPECT_DEATH(string_new_copy(&local, orig), "Pointer not empty");
}

TEST(String, NewF) {
  char *_s = null;
  char *local;

  string_new_f(&_s, "%d %c %.1f", 42, 'j', 3.14159);
  local = _s;
  string_delete(&_s);
  EXPECT_DEATH(string_new_f(&local, ""), "Pointer not empty");
}

TEST(String, NewConcat) {
  char *_s = null;
  char a[] = "Hi there";
  char b[] = "big spender";
  char *_c = null;

  new(&_c, 2);
  _c[0] = '!';
  _c[1] = '\0';
  string_new_concat_until_null(&_s, a, " - ", b, _c, null);
  EXPECT_DEATH(string_new_concat_until_null(&_s, null), "Pointer not empty");
  string_delete(&_s);
}

TEST(String, NewTruncate) {
  char *_s = null;

  EXPECT_DEATH(
      string_new_truncate(&_s, "cat", 4),
      "Cannot truncate string 'cat' to a length of 4 characters");
  EXPECT_DEATH(
      string_new_truncate(&_s, "", 1),
      "Cannot truncate string '' to a length of 1 character");
}

TEST(String, NewShorten) {
  char *_s = null;

  EXPECT_DEATH(
      string_new_shorten(&_s, "cat", 4),
      "Cannot shorten string 'cat' by 4 characters");
  EXPECT_DEATH(
      string_new_shorten(&_s, "", 1),
      "Cannot shorten string '' by 1 character");
}

TEST(String, RenewShorten) {
  char *_s = null;

  string_new_copy(&_s, "cat");
  EXPECT_DEATH(
      string_renew_shorten(&_s, 4),
      "Cannot shorten string 'cat' by 4 characters");
  string_delete(&_s);

  string_new_copy(&_s, "");
  EXPECT_DEATH(
      string_renew_shorten(&_s, 1),
      "Cannot shorten string '' by 1 character");
  string_delete(&_s);
}

TEST(String, RenewShortenBothEnds) {
  char *_s = null;

  string_new_copy(&_s, "cat");
  EXPECT_DEATH(
      string_renew_shorten_both_ends(&_s, 2),
      "Cannot shorten string 'cat' by 2 characters at both ends");
  string_delete(&_s);

  string_new_copy(&_s, "");
  EXPECT_DEATH(
      string_renew_shorten_both_ends(&_s, 1),
      "Cannot shorten string '' by 1 character at both ends");
  string_delete(&_s);
}

TEST(String, RenewShortenFromStart) {
  char *_s = null;

  string_new_copy(&_s, "cat");
  EXPECT_DEATH(
      string_renew_shorten_from_start(&_s, 4),
      "Cannot shorten string 'cat' by 4 characters");
  string_delete(&_s);

  string_new_copy(&_s, "");
  EXPECT_DEATH(
      string_renew_shorten_from_start(&_s, 1),
      "Cannot shorten string '' by 1 character");
  string_delete(&_s);
}

TEST(String, NewSubstring) {
  char *_s = null;

  EXPECT_DEATH(
      string_new_substring(&_s, "", 0, 1),
      "First index 0, length 1 invalid for '' (length 0)");
  EXPECT_DEATH(
      string_new_substring(&_s, "", 0, 2),
      "First index 0, length 2 invalid for '' (length 0)");
  EXPECT_DEATH(
      string_new_substring(&_s, "", 1, 1),
      "First index 1, length 1 invalid for '' (length 0)");
  string_new_substring(&_s, "a", 0, 1);
  EXPECT_STREQ("a", _s);
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_substring(&_s, "a", 1, 1),
      "First index 1, length 1 invalid for 'a' (length 1)");
  EXPECT_DEATH(
      string_new_substring(&_s, "a", 0, 2),
      "First index 0, length 2 invalid for 'a' (length 1)");
  string_delete(&_s);
  EXPECT_DEATH(
      string_new_substring(&_s, "ab", 2, 1),
      "First index 2, length 1 invalid for 'ab' (length 2)");
  EXPECT_DEATH(
      string_new_substring(&_s, "ab", 1, 2),
      "First index 1, length 2 invalid for 'ab' (length 2)");
  EXPECT_DEATH(
      string_new_substring(&_s, "ab", 0, 3),
      "First index 0, length 3 invalid for 'ab' (length 2)");
}

TEST(String, NewUnescape) {
  char *_s = null;

  EXPECT_DEATH(
      string_new_unescape(&_s, "\n"),
      "Illegal nonprinting character 0x0a found in string; escaped: '\\n'");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "\1"),
      "Illegal nonprinting character 0x01 found in string; escaped: '\\001'");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "\x1f"),
      "Illegal nonprinting character 0x1f found in string; escaped: '\\037'");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "\x7f"),
      "Illegal nonprinting character 0x7f found in string; escaped: '\\177'");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "\x80"),
      "Illegal nonprinting character 0x80 found in string; escaped: '\\200'");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "\xff"),
      "Illegal nonprinting character 0xff found in string; escaped: '\\377'");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "a\\0b"),
      "String 'a\\0b' contains illegal null escape '\\0' within the string");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "a\\00b"),
      "String 'a\\00b' contains illegal null escape '\\00' within the string");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "\\000b"),
      "String '\\000b' contains illegal null escape '\\000' within the string");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "a\\400b"),
      "String 'a\\400b' contains octal escape sequence '\\400' which has a "
      "value (0x100) that exceeds the maximum possible for a character (0xff)");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "a\\777b"),
      "String 'a\\777b' contains octal escape sequence '\\777' which has a "
      "value (0x1ff) that exceeds the maximum possible for a character (0xff)");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "a\\x12bz"),
      "String 'a\\x12bz' contains invalid hex escape sequence '\\x12b' "
      "of more than two hexits");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "a\\X12Bz"),
      "String 'a\\X12Bz' contains invalid hex escape sequence '\\X12B' "
      "of more than two hexits");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "a\\XDEFz"),
      "String 'a\\XDEFz' contains invalid hex escape sequence '\\XDEF' "
      "of more than two hexits");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "a\\x0z"),
      "String 'a\\x0z' contains illegal null escape '\\x0' within the string");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "a\\X0z"),
      "String 'a\\X0z' contains illegal null escape '\\X0' within the string");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "a\\x00z"),
      "String 'a\\x00z' contains illegal null escape '\\x00' within the "
      "string");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "a\\X00z"),
      "String 'a\\X00z' contains illegal null escape '\\X00' within the "
      "string");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "a\\cb"),
      "Invalid escape sequence '\\c' in string 'a\\cb'");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "a\\8b"),
      "Invalid escape sequence '\\8' in string 'a\\8b'");
  string_delete(&_s);

  EXPECT_DEATH(
      string_new_unescape(&_s, "a\\9b"),
      "Invalid escape sequence '\\9' in string 'a\\9b'");
  string_delete(&_s);
}

TEST(String, MaxLen) {
  /* Invalid format strings.
   */
  EXPECT_DEATH(
      string_f_get_max_len("%--d"),
      "'-' was already specified in format string");
  EXPECT_DEATH(
      string_f_get_max_len("%++d"),
      "'+' was already specified in format string");
  EXPECT_DEATH(
      string_f_get_max_len("%  d"),
      "' ' was already specified in format string");
  EXPECT_DEATH(
      string_f_get_max_len("% +d"),
      "Can't specify both '+' and ' ' in format string");
  EXPECT_DEATH(
      string_f_get_max_len("%+ d"),
      "Can't specify both '+' and ' ' in format string");
  EXPECT_DEATH(
      string_f_get_max_len("%00d"),
      "'0' was already specified in format string");
  EXPECT_DEATH(
      string_f_get_max_len("%##d"),
      "'#' was already specified in format string");
  EXPECT_DEATH(
      string_f_get_max_len("%..d"),
      "Character '.' at index 2 of format string '%..d' is invalid");
  EXPECT_DEATH(
      string_f_get_max_len("%D"),
      "Character 'D' at index 1 of format string '%D' is invalid");
  EXPECT_DEATH(
      string_f_get_max_len("%Ld"),
      "Format 'Ld' is invalid");
  EXPECT_DEATH(
      string_f_get_max_len("%hf"),
      "Format 'hf' is invalid");
  EXPECT_DEATH(
      string_f_get_max_len("%lf"),
      "Format 'lf' is invalid");
}

TEST(String, Copy) {
  char s[10];
  char *e = "";

  e = "a";
  EXPECT_DEATH(string_copy(e, s, 0), "max_len 0 but string length 1");
  string_copy(e, s, 1);
  EXPECT_STREQ(e, s);
  string_copy(e, s, 2);
  EXPECT_STREQ(e, s);
  e = "ab";
  EXPECT_DEATH(string_copy(e, s, 0), "max_len 0 but string length 2");
  EXPECT_DEATH(string_copy(e, s, 1), "max_len 1 but string length 2");
  string_copy(e, s, 2);
  EXPECT_STREQ(e, s);
  string_copy(e, s, 3);
  EXPECT_STREQ(e, s);
  e = "123456789";
  EXPECT_DEATH(string_copy(e, s, 8), "max_len 8 but string length 9");
}

TEST(String, Break) {
  char *_s = null;

  EXPECT_DEATH(
      string_new_copy_wrap(&_s, "Hi", 0, 0, 2, null),
      "Width cannot be zero");
  string_delete(&_s);
  EXPECT_DEATH(
      string_new_copy_wrap(&_s, "Hi", 11, 10, 2, null),
      "Start 11 not valid for width 10");
  string_delete(&_s);
  EXPECT_DEATH(
      string_new_copy_wrap(&_s, "Hi", 10, 10, 2, null),
      "Start 10 not valid for width 10");
  EXPECT_DEATH(
      string_new_copy_wrap(&_s, "Hi", 0, 10, 11, null),
      "Indent 11 not valid for width 10");
  EXPECT_DEATH(
      string_new_copy_wrap(&_s, "Hi", 0, 10, 10, null),
      "Indent 10 not valid for width 10");
  EXPECT_DEATH(
      string_new_copy_wrap(&_s, "Hi\n", 0, 10, 0, null),
      "Cannot wrap strings with newlines");
  EXPECT_DEATH(
      string_new_copy_wrap(&_s, "\nHi", 0, 10, 0, null),
      "Cannot wrap strings with newlines");
  EXPECT_DEATH(
      string_new_copy_wrap(&_s, "Hi\nthere", 0, 10, 0, null),
      "Cannot wrap strings with newlines");
  string_delete(&_s);
}

FINISH_DEATH
