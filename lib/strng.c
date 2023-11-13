#include "strng.h"
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

#define INDENT_ 2
#define INDENT_S_ ". "

static int min_exponent_(double d);

static int max_exponent_(double d);

static void path_to_directory_filename_(
    char *path,
    char **directory,
    char **filename);

static void new_escape_(char **p_this, char *other, bool for_logging);


void string_new_empty(char **p_this) {
  newo(p_this);
  **p_this = '\0';
}

void string_new_char(char **p_this, char c) {
  char *this;

  new(p_this, 2);
  this = *p_this;
  this[0] = c;
  this[1] = '\0';
}

void string_new_copy(char **p_this, char *other) {
  ulong len;
  char *this;

  len = string_length(other);

  new(p_this, len + 1);
  this = *p_this;

  string_copy(other, this, len);
}

void string_new_load(char **p_this, File *f) {
  ulong len;

  load_value(len, f);

  if (len == ULONG_MAX) {
    return;
  }

  string_new_load_from_length(p_this, len, f);
}

void string_new_load_from_length(char **p_this, ulong length, File *f) {
  char *this;

  new(p_this, length + 1);
  this = *p_this;

  load_values(this, length, f);
  this[length] = '\0';
}

void string_new_copy_lowercase(char **p_this, char *other) {
  char *e;
  char *p;
  char *q;
  ulong len;
  char *this;

  len = string_length(other);

  new(p_this, len + 1);
  this = *p_this;

  for (p = this, e = p + len, q = other; p <= e; p++, q++) {
    *p = tolower(*q);
  }
}

void string_new_f(char **p_this, char *fmt, ...) {
  va_list ap1;
  va_list ap2;

  va_start(ap1, fmt);
  va_start(ap2, fmt);
  string_new_vf(p_this, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);
}

void string_new_vf(char **p_this, char *fmt, va_list ap1, va_list ap2) {
  ulong max_len;
  char *this;

  max_len = string_vf_get_max_len(fmt, ap1);

  new(p_this, max_len + 1);
  this = *p_this;

  string_vf_print_into(this, fmt, ap2, max_len);

  string_renew_trim_length(p_this, max_len);
}

void string_new_concat(char **p_this, char *first, char *second) {
  string_new_concat_until_null(p_this, first, second, null);
}

void string_new_concat_until_null(char **p_this, ...) {
  char *s;
  char *p;
  char *e;
  ulong len;
  va_list ap;
  char *this;

  len = 0;

  va_start(ap, p_this);

  while ((s = va_arg(ap, char *)) != null) {
    len += string_length(s);
  }

  va_end(ap);

  new(p_this, len + 1);
  this = *p_this;

  va_start(ap, p_this);

  for (p = this, e = p + len; (s = va_arg(ap, char *)) != null;) {
    p = string_copy(s, p, e - p);
  }

  va_end(ap);
}

void string_new_concat_array(char **p_this, ulong n, char **others) {
  char *e;
  char *p;
  ulong len;
  ulong u;
  char *this;

  len = 0;

  for (u = 0; u < n; u++) {
    len += string_length(others[u]);
  }

  new(p_this, len + 1);
  this = *p_this;

  for (u = 0, p = this, e = p + len; u < n; u++) {
    p = string_copy(others[u], p, e - p);
  }
}

void string_new_truncate(char **p_this, char *other, ulong len) {
  ulong other_len;
  char *this;

  other_len = string_length(other);

  if (len > other_len) {
    throw("Cannot truncate string '%s' to a length of %lu character%s",
        other,
        len,
        plural_s(len));
  }

  new(p_this, len + 1);
  this = *p_this;
  memcpy(this, other, len);
  this[len] = '\0';
}

void string_new_shorten(char **p_this, char *other, ulong n) {
  ulong other_len;
  ulong len;
  char *this;

  other_len = string_length(other);

  if (n > other_len) {
    throw("Cannot shorten string '%s' by %lu character%s",
        other,
        n,
        plural_s(n));
  }

  len = other_len - n;
  new(p_this, len + 1);
  this = *p_this;
  memcpy(this, other, len);
  this[len] = '\0';
}

void string_new_indent(char **p_this, char *other, ulong n) {
  string_new_copy(p_this, other);
  string_renew_indent(p_this, n);
}

void string_new_filename_from_path(char **p_this, char *path) {
  path_to_directory_filename_(path, null, p_this);
}

void string_renew_filename_from_path(char **p_this, char *path) {
  string_delete(p_this);
  string_new_filename_from_path(p_this, path);
}

void string_new_directory_from_path(char **p_this, char *path) {
  path_to_directory_filename_(path, p_this, null);
}

void string_new_standardize_directory(char **p_this, char *directory) {
  bool had;
  char c;
  char *p;
  char *q;
  ulong old_len;
  char *this;

  old_len = string_length(directory);
  new(p_this, old_len + 1);
  this = *p_this;

  for (p = directory, q = this, had = false; (c = *p) != '\0'; p++) {
    if (had) {
      if (c == '/') {
        continue;
      }

      had = false;
    } else if (c == '/') {
      had = true;
    }

    *q++ = c;
  }

  if (had && q - this > 1) {
    q--;
  }

  *q = '\0';

  string_renew_trim_length(p_this, old_len);

  if (string_is_empty(*p_this)) {
    string_renew_copy(p_this, ".");
  }
}

void string_renew_standardize_directory(char **p_this) {
  char *_s = null;

  string_new_standardize_directory(&_s, *p_this);
  string_delete(p_this);
  donate(&_s, p_this);
}

void strings_new_directory_filename_from_path(
    char **directory,
    char **filename,
    char *path) {
  path_to_directory_filename_(path, directory, filename);
}

void string_new_extension_from_path(char **p_this, char *path) {
  bool gzipped;

  string_new_extension_gzipped_from_path(p_this, &gzipped, path);
}

void string_new_extension_gzipped_from_path(
    char **p_this,
    bool *gzipped,
    char *path) {
  long index;
  char *_s = null;

  string_new_filename_from_path(&_s, path);

  if (string_ends_with(_s, ".gz", true)) {
    *gzipped = true;
    string_renew_shorten(&_s, 3);
  } else {
    *gzipped = false;
  }

  index = string_last_index_of(_s, '.');

  if (index > 0) {
    string_new_copy_lowercase(p_this, _s + index + 1);
  }

  string_delete(&_s);
}

void string_new_remove_extension(char **p_this, char *path) {
  long dot_index;
  long slash_index;
  char *_s = null;

  string_new_copy(&_s, path);

  if (string_ends_with(_s, ".gz", true)) {
    string_renew_shorten(&_s, 3);
  }

  dot_index = string_last_index_of(_s, '.');
  slash_index = string_last_index_of(_s, '/');

  if (dot_index >= 0 && dot_index > slash_index) {
    string_new_truncate(p_this, _s, dot_index);
  } else {
    string_new_copy(p_this, _s);
  }

  string_delete(&_s);
}

void string_new_trim_whitespace(char **p_this, char *other) {
  char *e;
  char *p;
  char *q;
  char *this;

  for (p = other; isspace(*p); p++) {
  }

  for (e = p; *e != '\0'; e++) {
  }

  if (e > p) {
    for (e--; isspace(*e); e--) {
    }

    e++;
  }

  new(p_this, e - p + 1);
  this = *p_this;

  for (q = this; p < e; p++, q++) {
    *q = *p;
  }

  *q = '\0';
}

void string_renew_trim_whitespace(char **p_this) {
  char *_other = null;

  if (**p_this == '\0') {
    return;
  }

  if (!isspace((*p_this)[0])
      && !isspace((*p_this)[string_length(*p_this) - 1])) {
    return;
  }

  donate(p_this, &_other);
  string_new_trim_whitespace(p_this, _other);
  string_delete(&_other);
}

void string_new_substring(
    char **p_this,
    char *other,
    ulong first_index,
    ulong length) {
  char *p;
  char *q;
  char *e;
  char *this;

  if (first_index + length > string_length(other)) {
    throw("First index %lu, length %lu invalid for '%s' (length %lu)",
        first_index,
        length,
        other,
        string_length(other));
  }

  new(p_this, length + 1);
  this = *p_this;

  for (
      p = this, q = other + first_index, e = q + length;
      q < e;
      p++, q++) {
    *p = *q;
  }

  *p = '\0';
}

void string_new_join(char **p_this, ulong n, char **others, char *join) {
  char *e;
  char *p;
  ulong u;
  ulong len;
  ulong join_len;
  char *this;

  if (n == 0) {
    string_new_empty(p_this);

    return;
  }

  join_len = n > 1 ? string_length(join) : 0;

  for (u = 0, len = (n - 1) * join_len; u < n; u++) {
    len += string_length(others[u]);
  }

  this = new(p_this, len + 1);

  for (u = 0, p = this, e = p + len; u < n; u++) {
    if (u > 0) {
      p = string_copy(join, p, e - p);
    }

    p = string_copy(others[u], p, e - p);
  }
}

void string_new_replace_chars(
    char **p_this,
    char *other,
    char *find,
    char replace) {
  char *this;

  string_new_copy(p_this, other);
  this = *p_this;
  string_replace_chars(this, find, replace);
}

void string_new_replace_char_with_string(
    char **p_this,
    char *other,
    char find,
    char *replace) {
  char c;
  char *p;
  char *q;
  char *r;
  char *t;
  ulong len;
  char *this;

  string_new_empty(p_this);

  for (p = other;; p = q) {
    for (q = p; (c = *q) != '\0' && c != find; q++) {
    }

    this = *p_this;

    for (r = this; *r != '\0'; r++) {
    }

    len = r - this;
    renewup(p_this, len + 1, len + q - p + 1);

    for (r = *p_this + len, t = p; t < q; t++, r++) {
      *r = *t;
    }

    *r = '\0';

    if (c == '\0') {
      break;
    }

    q++;

    if (*q == find) {
      string_renew_append_char(p_this, find);
      q++;
    } else {
      string_renew_append(p_this, replace);
    }
  }
}

void string_new_escape(char **p_this, char *other) {
  new_escape_(p_this, other, false);
}

void string_new_escape_for_logging(char **p_this, char *other) {
  new_escape_(p_this, other, true);
}

void string_renew_escape_for_logging(char **p_this) {
  char *_s = null;

  string_new_escape_for_logging(&_s, *p_this);
  string_delete(p_this);
  donate(&_s, p_this);
}

void string_new_unescape(char **p_this, char *other) {
  char c;
  char x;
  char buf[5];
  char *b;
  char *endp;
  char *p;
  char *q;
  char *r;
  char *_e = null;
  ulong max_len;
  ulong u;

  max_len = string_length(other);

  new(p_this, max_len + 1);

  for (p = *p_this, q = other; (c = *q) != '\0'; q++) {
    if (!isprint(c)) {
      string_new_escape(&_e, other);
      throw("Illegal nonprinting character 0x%02x found in string; "
          "escaped: '%s'",
          (byte)c,
          _e);
    }

    if (c == '\\') {
      switch(c = *++q) {
        case 'a':
          *p++ = '\a';
          break;
        case 'b':
          *p++ = '\b';
          break;
        case 'f':
          *p++ = '\f';
          break;
        case 'n':
          *p++ = '\n';
          break;
        case 'r':
          *p++ = '\r';
          break;
        case 't':
          *p++ = '\t';
          break;
        case 'v':
          *p++ = '\v';
          break;
        case '\\':
          *p++ = '\\';
          break;
        case '?':
          *p++ = '?';
          break;
        case '\'':
          *p++ = '\'';
          break;
        case '"':
          *p++ = '"';
          break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
          b = r = buf;

          if (c != '0') {
            *r++ = '0';
            b++;
          }

          *r++ = c;

          if (isdigit(c = q[1]) && c != '8' && c != '9') {
            *r++ = c;
            q++;
          }

          if (isdigit(c = q[1]) && c != '8' && c != '9') {
            *r++ = c;
            q++;
          }

          *r = '\0';
          u = strtoul(buf, &endp, 0);

          if (u == 0) {
            deletev(p_this);
            throw("String '%s' contains illegal null escape '\\%s' "
                "within the string",
                other,
                b);
          }

          if (*endp != '\0') {
            deletev(p_this);
            throw("Impossible");
          }

          if (u > UCHAR_MAX) {
            deletev(p_this);
            throw("String '%s' contains octal escape sequence '\\%s' which "
                "has a value (0x%x) that exceeds the maximum possible for a "
                "character (0x%x)",
                other,
                b,
                u,
                UCHAR_MAX);
          }

          *p++ = (char)(byte)u;

          break;

        case 'x':
        case 'X':
          r = buf;
          *r++ = '0';
          *r++ = x = c;
          b = r;

          if (isxdigit(c = q[1])) {
            *r++ = c;
            q++;
          }

          if (isxdigit(c = q[1])) {
            *r++ = c;
            q++;
          }

          if (isxdigit(c = q[1])) {
            deletev(p_this);
            throw("String '%s' contains invalid hex escape sequence "
                "'\\%c%s%c' of more than two hexits",
                other,
                x,
                b,
                c);
          }

          *r = '\0';
          u = strtoul(buf, &endp, 0);

          if (u == 0) {
            deletev(p_this);
            throw("String '%s' contains illegal null escape '\\%c%s' "
                "within the string",
                other,
                x,
                b);
          }

          if (u > UCHAR_MAX || *endp != '\0') {
            deletev(p_this);
            throw("Impossible");
          }

          *p++ = (char)(byte)u;

          break;

        default:
          deletev(p_this);

          if (c == '\0') {
            throw("String '%s' ended with hanging escape backslash", other);
          }

          throw("Invalid escape sequence '\\%c' in string '%s'", c, other);

          break;
      }
    } else {
      *p++ = c;
    }
  }

  *p = '\0';

  string_renew_trim_length(p_this, max_len);
}

void string_new_escape_char(char **p_this, char c) {
  char s[2];

  s[0] = c;
  s[1] = '\0';
  string_new_escape(p_this, s);
}

void string_new_quote_or_null(char **p_this, char *other, char *null_string) {
  if (other == null) {
    string_new_copy(p_this, null_string);

    return;
  }

  string_new_f(p_this, "'%s'", other);
}

void string_new_add_separators(char **p_this, char *other) {
  bool dec;
  bool e;
  bool veto;
  char c;
  char *p;
  char *q;
  char *r;
  char *start;
  ulong u;

  string_new_empty(p_this);

  /* p and q point to the input string; r points to the output string, but
   * because it can be renewed, u keeps track of the current index in the
   * output string.
   *
   * Each outer loop first looks for the next part of the input string that is
   * not a number, and appends that to the output string. It then looks for the
   * part of the input string that is a number. If its integral part has four
   * digits or less, it is just appended to the input string; otherwise, it is
   * appended with thousands separators inserted. Then the outer loop iterates.
   */
  for (u = 0, p = other, veto = false, dec = false; *p != '\0';) {
    /* Find the part of the input string that is not a number. We keep running
     * q while the characters are non-digits, or while the 'veto' is in place,
     * which is true if we have an alpha character or '_', or if we already
     * have a veto running and find a digit or '-'. If the last character is a
     * decimal point, we save that information for the next step.
     */
    for (q = p; (c = *q) != '\0' && (veto || !isdigit(c)); q++) {
      veto = isalpha(c) || c == '_' || (veto && (isdigit(c) || c == '-'));
      dec = c == '.';
    }

    /* Lengthen the output string to store this non-number part, copy it over,
     * and terminate it.
     */
    renewup(p_this, u + 1, u + (q - p) + 1);

    for (r = *p_this + u; p != q; p++, r++, u++) {
      *r = *p;
    }

    *r = '\0';

    /* Now run q until we hit a non-digit.
     */
    for (; (c = *q) != '\0' && isdigit(c); q++) {
    }

    /* We now have three cases.
     *
     * If we are not following a decimal point, and we have just a single digit
     * that is '0', and it is followed by 'x' or 'X', then we have a potential
     * hex number.
     *
     * Otherwise, if we are not following a decimal point and the first digit
     * is '0', or the digits are followed immediately by an underscore or alpha
     * character that is not 'e' or 'E', or the number of digits is less than
     * five, just copy them across.
     *
     * Otherwise, lengthen the output string with enough extra characters for
     * the separators, and copy the digits across, inserting the separators.
     */
    if (!dec && q == p + 1 && *p == '0' && tolower(c) == 'x') {
      /* Potential hex number. Run q until we don't have a hex digit.
       */
      for (q++; (c = *q) != '\0' && isxdigit(c); q++) {
      }

      /* Check if we have less than five digits (note that we also have the
       * '0x' in this string), or if the hex digits are followed by a non-hex
       * alpha character or underscore; if either is true, just copy the
       * characters. Otherwise, insert spaces every four hex digits, as well as
       * one after the initial '0x'.
       */
      if (q - p < 7 || isalpha(c) || c == '_') {
        renewup(p_this, u + 1, u + (q - p) + 1);

        for (r = *p_this + u; p != q; p++, r++, u++) {
          *r = *p;
        }
      } else {
        renewup(p_this, u + 1, u + (q - p) + (q - p - 3) / 4 + 2);
        start = p;

        for (r = *p_this + u; p != q; p++, r++, u++) {
          if (p == start + 2 || (p > start + 2 && (q - p) % 4 == 0)) {
            *r++ = ' ';
            u++;
          }

          *r = *p;
        }
      }
    } else if ((!dec && *p == '0') || (isalpha(c) && tolower(c) != 'e')
        || c == '_' || q - p < 5) {
      renewup(p_this, u + 1, u + (q - p) + 1);

      for (r = *p_this + u; p != q; p++, r++, u++) {
        *r = *p;
      }
    } else {
      e = dec && tolower(c) == 'e';
      renewup(p_this,
          u + 1,
          u + (q - p) + (q - p - 1) / 3 + 1 + (e ? 1 : 0));
      start = p;

      for (r = *p_this + u; p != q; p++, r++, u++) {
        if (dec) {
          if (p != start && (p - start) % 3 == 0) {
            *r++ = ' ';
            u++;
          }

        } else {
          if (p != start && (q - p) % 3 == 0) {
            *r++ = ',';
            u++;
          }
        }

        *r = *p;
      }

      if (e) {
        *r++ = ' ';
        u++;
      }
    }

    /* Terminate the output string.
     */
    *r = '\0';
  }
}

void string_renew_add_separators(char **p_this) {
  char *_s = null;

  string_new_add_separators(&_s, *p_this);
  string_delete(p_this);
  donate(&_s, p_this);
}

void string_renew_trim_length(char **p_this, ulong old_length) {
  ulong new_length;

  new_length = string_length(*p_this);

  renewv(p_this, old_length + 1, new_length + 1);
}

void string_renew_append(char **p_this, char *other) {
  string_renew_append_until_null(p_this, other, null);
}

void string_renew_append_until_null(char **p_this, ...) {
  char *s;
  char *p;
  char *e;
  ulong old_len;
  ulong len;
  va_list ap;
  char *this;

  old_len = string_length(*p_this);

  va_start(ap, p_this);

  for (len = old_len; (s = va_arg(ap, char *)) != null;) {
    len += string_length(s);
  }

  va_end(ap);

  renewup(p_this, old_len + 1, len + 1);
  this = *p_this;

  va_start(ap, p_this);

  for (p = this + old_len, e = this + len; (s = va_arg(ap, char*)) != null;) {
    p = string_copy(s, p, e - p);
  }

  va_end(ap);
}

void string_renew_append_char(char **p_this, char c) {
  ulong len;
  char *this;

  len = string_length(*p_this);
  renewup1(p_this, len + 1);
  this = *p_this;

  this[len] = c;
  this[len + 1] = '\0';
}

void string_renew_append_f(char **p_this, char *fmt, ...) {
  va_list ap1;
  va_list ap2;

  va_start(ap1, fmt);
  va_start(ap2, fmt);
  string_renew_append_vf(p_this, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);
}

void string_renew_append_vf(
    char **p_this,
    char *fmt,
    va_list ap1,
    va_list ap2) {
  ulong old_len;
  ulong new_len;
  ulong max_add_len;
  char *this;

  old_len = string_length(*p_this);

  max_add_len = string_vf_get_max_len(fmt, ap1);
  new_len = old_len + max_add_len;

  renewup(p_this, old_len + 1, new_len + 1);
  this = *p_this;

  string_vf_print_into(this + old_len, fmt, ap2, max_add_len);

  string_renew_trim_length(p_this, new_len);
}

void string_renew_prepend(char **p_this, char *prefix) {
  char *src;
  char *dst;
  ulong old_len;
  ulong new_len;
  ulong prefix_len;
  char *this;

  old_len = string_length(*p_this);
  prefix_len = string_length(prefix);
  new_len = old_len + prefix_len;

  renewup(p_this, old_len + 1, new_len + 1);
  this = *p_this;

  for (src = this + old_len, dst = this + new_len; src >= this; src--, dst--) {
    *dst = *src;
  }

  for (src = prefix + prefix_len - 1; src >= prefix; src--, dst--) {
    *dst = *src;
  }
}

void string_renew_prepend_f(char **p_this, char *fmt, ...) {
  char *_final = null;
  char *_prepend = null;
  ulong len;
  va_list ap;

  va_start(ap, fmt);
  len = string_vf_get_max_len(fmt, ap);
  va_end(ap);

  new(&_prepend, len + 1);

  va_start(ap, fmt);
  string_vf_print_into(_prepend, fmt, ap, len);
  va_end(ap);

  string_new_concat_until_null(&_final, _prepend, *p_this, null);
  string_delete(p_this);

  string_delete(&_prepend);

  donate(&_final, p_this);
}

void string_renew_ensure_ends_with(
    char **p_this,
    char *other,
    bool ignore_case) {
  if (string_ends_with(*p_this, other, ignore_case)) {
    return;
  }

  string_renew_append(p_this, other);
}

void string_renew_copy(char **p_this, char *other) {
  string_delete(p_this);
  string_new_copy(p_this, other);
}

void string_renew_shorten(char **p_this, ulong n) {
  ulong len;

  len = string_length(*p_this);

  if (n > len) {
    throw("Cannot shorten string '%s' by %lu character%s",
        *p_this,
        n,
        plural_s(n));
  }

  renewv(p_this, len + 1, len + 1 - n);
  (*p_this)[len - n] = '\0';
}

void string_renew_shorten_both_ends(char **p_this, ulong n) {
  char *this;
  ulong len;
  ulong u;

  this = *p_this;
  len = string_length(this);

  if (2 * n > len) {
    throw("Cannot shorten string '%s' by %lu character%s at both ends",
        *p_this,
        n,
        plural_s(n));
  }

  for (u = 0; u < len - n; u++) {
    this[u] = this[u + n];
  }

  renewv(p_this, len + 1, len + 1 - 2 * n);

  (*p_this)[len - 2 * n] = '\0';
}

void string_renew_shorten_from_start(char **p_this, ulong n) {
  char *this;
  ulong len;
  ulong u;

  this = *p_this;
  len = string_length(this);

  if (n > len) {
    throw("Cannot shorten string '%s' by %lu character%s",
        *p_this,
        n,
        plural_s(n));
  }

  for (u = 0; u < len - n; u++) {
    this[u] = this[u + n];
  }

  renewv(p_this, len + 1, len + 1 - n);

  (*p_this)[len - n] = '\0';
}

void string_renew_indent(char **p_this, ulong n) {
  char c;
  char *p;
  char *src;
  char *dst;
  ulong num_lines;
  ulong old_len;
  ulong new_len;
  ulong u;
  ulong v;
  char *this;

  if (n == 0) {
    return;
  }

  for (p = *p_this, num_lines = 1; (c = *p) != '\0'; p++) {
    if (c == '\n') {
      num_lines++;
    }
  }

  old_len = p - *p_this;
  new_len = old_len + n * INDENT_ * num_lines;

  renewup(p_this, old_len + 1, new_len + 1);
  this = *p_this;

  for (src = this + old_len, dst = this + new_len; src >= this; src--) {
    if ((c = *src) == '\n') {
      for (u = 0; u < n; u++) {
        for (v = 0, p = &INDENT_S_[INDENT_ - 1]; v < INDENT_; v++, p--) {
          *dst-- = *p;
        }
      }
    }

    *dst-- = c;
  }

  for (u = 0; u < n; u++) {
    for (v = 0, p = &INDENT_S_[INDENT_ - 1]; v < INDENT_; v++, p--) {
      *dst-- = *p;
    }
  }

  if (dst + 1 != this) {
    throw("impossible");
  }
}

void strings_new_split(
    char ***p_this,
    ulong *num_results,
    char *other,
    char split_char) {
  char c;
  char *p;
  char *s;
  ulong n;
  ulong u;
  char **this;

  for (n = 0, p = other; (c = *p) != '\0'; p++) {
    if (c == split_char) {
      n++;
    }
  }

  n++;

  new(p_this, *num_results = n);
  this = *p_this;

  for (u = 0, s = p = other; (c = *p) != '\0'; p++) {
    if (c == split_char) {
      string_new_substring(&this[u++], s, 0, p - s);
      s = p + 1;
    }
  }

  string_new_substring(&this[u++], s, 0, p - s);

  if (u != n) {
    throw("Impossible");
  }
}

void string_delete(char **p_this) {
  char *this;

  if ((this = *p_this) == null) {
    return;
  }

  deletev(p_this);
}

void string_print(char *this, Printer *p) {
  char c;
  char *q;
  char *_s1 = null;
  char *_s2 = null;
  char *_s3 = null;
  char *_s4 = null;

  sprint(p, "String");

  if (string_contains_nonprintable(this)) {
    rprint(p, "Length: %lu", string_length(this));
    rprint(p, "String contains nonprintable characters");
    string_new_empty(&_s1);
    string_new_empty(&_s2);
    string_new_escape(&_s3, this);
    string_new_empty(&_s4);

    for (q = this; (c = *q) != '\0'; q++) {
      if (isprint(c)) {
        string_renew_append_char(&_s1, c);
        string_renew_append_char(&_s2, c);
      } else {
        string_renew_append_char(&_s2, '.');
      }

      if (q != this) {
        string_renew_append_char(&_s4, ' ');
      }

      string_renew_append_f(&_s4, "%02x", (byte)c);
    }

    rprint(p, "Nonprintable removed: '%s'", _s1);
    rprint(p, "Nonprintable replaced by dots: '%s'", _s2);
    rprint(p, "Escaped: '%s'", _s3);
    rprint(p, "Hex: %s", _s4);
    string_delete(&_s4);
    string_delete(&_s3);
    string_delete(&_s2);
    string_delete(&_s1);
  } else {
    rprint(p, "Length: %lu", string_length(this));
    rprint(p, "Contents: '%s'", this);
  }

  fprint(p);
}

void string_save(char *this, File *f) {
  ulong len;

  if (this == null) {
    len = ULONG_MAX;
    save_value(len, f);
  } else {
    len = string_length(this);
    save_value(len, f);
    save_values(this, len, f);
  }
}

bool string_is_empty(char *this) {
  return *this == '\0';
}

ulong string_length(char *this) {
  return strlen(this);
}

bool string_equals(char *this, char *other) {
  return strcmp(this, other) == 0;
}

sbyte string_compare(char *a, char *b, void *x) {
  int c;

  c = strcmp(a, b);

  return c < 0 ? -1 : c > 0 ? +1 : 0;
}

sbyte string_pointer_compare(char **a, char **b, void *x) {
  return string_compare(*a, *b, x);
}

bool string_contains_nonprintable(char *this) {
  char c;
  char *p;

  for (p = this; (c = *p) != '\0'; p++) {
    if (!isprint(c)) {
      return true;
    }
  }

  return false;
}

bool string_contains_char(char *this, char c) {
  char d;
  char *p;

  for (p = this; (d = *p) != '\0'; p++) {
    if (d == c) {
      return true;
    }
  }

  return false;
}

bool string_contains_chars(char *this, char *s) {
  char c;
  char d;
  char *p;
  char *q;

  for (p = this; (d = *p) != '\0'; p++) {
    for (q = s; (c = *q) != '\0'; q++) {
      if (d == c) {
        return true;
      }
    }
  }

  return false;
}

bool string_starts_with(char *this, char *other, bool ignore_case) {
  char *t;
  char *o;

  if (ignore_case) {
    for (t = this, o = other;
        *t != '\0' && *o != '\0' && tolower(*t) == tolower(*o);
        t++, o++) {
    }
  } else {
    for (t = this, o = other; *t != '\0' && *o != '\0' && *t == *o; t++, o++) {
    }
  }

  return *o == '\0';
}

bool string_ends_with(char *this, char *other, bool ignore_case) {
  char *t;
  char *o;

  if (ignore_case) {
    for (t = this + string_length(this) - 1,
        o = other + string_length(other) - 1;
        t >= this && o >= other && tolower(*t) == tolower(*o);
        t--, o--) {
    }
  } else {
    for (t = this + string_length(this) - 1,
        o = other + string_length(other) - 1;
        t >= this && o >= other && *t == *o;
        t--, o--) {
    }
  }

  return o < other;
}

long string_first_index_of(char *this, char c) {
  return string_nth_index_of(this, 0, c);
}

long string_nth_index_of(char *this, ulong n, char c) {
  char *p;
  char *e;
  ulong k;

  for (k = 0, p = this, e = p + string_length(this); p < e; p++) {
    if (*p == c) {
      if (k++ == n) {
        return p - this;
      }
    }
  }
  return -1;
}


long string_last_index_of(char *this, char c) {
  return string_nth_last_index_of(this, 0, c);
}

long string_nth_last_index_of(char *this, ulong n, char c) {
  char *p;
  ulong k;

  for (k = 0, p = this + string_length(this) - 1; p >= this; p--) {
    if (*p == c) {
      if (k++ == n) {
        return p - this;
      }
    }
  }

  return -1;
}

ulong string_num_occurrences(char *this, char c) {
  char *p;
  ulong n;

  for (p = this, n = 0; *p != '\0'; p++) {
    if (*p == c) {
      n++;
    }
  }

  return n;
}

bool string_substring_equals(char *this, ulong start, ulong end, char *other) {
  char c;
  char d;
  char *p;
  char *pe;
  char *o;

  for (p = this + start, pe = this + end, o = other;
      (c = *p) != '\0' && p != pe;
      p++, o++) {
    if ((d = *o) == '\0') {
      return false;
    }

    if (d != c) {
      return false;
    }
  }

  return true;
}

bool string_contains_at(char *this, ulong index, char *other) {
  char c;
  char *p;
  char *q;
  ulong len_other;

  len_other = string_length(other);

  if (index + len_other > string_length(this)) {
    return false;
  }

  for (p = this + index, q = other; (c = *q) != '\0'; p++, q++) {
    if (*p != c) {
      return false;
    }
  }

  return true;
}

void string_replace_chars(char *this, char *find, char replace) {
  char *p;
  char *f;

  for (p = this; *p != '\0'; p++) {
    for (f = find; *f != '\0'; f++) {
      if (*p == *f) {
        *p = replace;
        break;
      }
    }
  }
}

void string_trim_trailing_whitespace(char *this) {
  char *p;

  for (p = this + string_length(this) - 1; p >= this && isspace(*p); p--) {
  }

  *(++p) = '\0';
}

void string_new_copy_wrap(
    char **p_this,
    char *other,
    ulong start,
    ulong width,
    ulong indent,
    bool *wrapped) {
  char c;
  char *last_q;
  char *p;
  char *q;
  char *t;
  char *u;
  char **_ss = null;
  ulong len;
  ulong n;
  ulong pos;
  ulong start_pos;

  if (width == 0) {
    throw("Width cannot be zero");
  }

  if (start >= width) {
    throw("Start %lu not valid for width %lu", start, width);
  }

  if (indent >= width) {
    throw("Indent %lu not valid for width %lu", indent, width);
  }

  if (wrapped != null) {
    *wrapped = false;
  }

  if (*other == '\0') {
    string_new_empty(p_this);

    return;
  }

  for (p = other; (c = *p) != '\0'; p++) {
    if (c == '\n') {
      throw("Cannot wrap strings with newlines");
    }
  }

  new(&_ss, n = 0);

  for (p = other, start_pos = start;; start_pos = indent) {
    q = p;

    do {
      last_q = q;

      while (*q == ' ') {
        q++;
      }

      for (; (c = *q) != '\0' && c != ' '; q++) {
      }

      len = q - p;

      if (len == 0) {
        throw("impossible");
      }

      pos = start_pos + len;

      if (c == '\0' || c == '\n') {
        break;
      }

      q++;
    } while (pos <= width);

    if (pos > width) {
      if (last_q == p) {
        q = p + width - start_pos;
      } else {
        q = last_q - 1;
      }

      len = q - p;
      c = *q;
    }

    renewup1(&_ss, n);
    new(&_ss[n], len + 1);
    u = _ss[n];
    n++;

    if (q == p) {
      throw("zero length");
    }

    for (t = p; t != q; t++, u++) {
      *u = *t;
    }

    *u = '\0';

    if (c == '\0') {
      break;
    }

    p = q;

    while (*p == ' ') {
      p++;
    }

    if (*p == '\0') {
      break;
    }

    renewup1(&_ss, n);
    string_new_f(&_ss[n++], "\n%*s", indent, "");

    if (wrapped != null) {
      *wrapped = true;
    }
  }

  string_new_concat_array(p_this, n, _ss);
  delete(&_ss, n, string_delete);
}

void string_renew_wrap(
    char **p_this,
    ulong start,
    ulong width,
    ulong indent,
    bool *wrapped) {
  char *_tmp = null;

  string_new_copy_wrap(&_tmp, *p_this, start, width, indent, wrapped);
  string_delete(p_this);
  donate(&_tmp, p_this);
}

void string_renew_append_wrap(
    char **p_this,
    char *other,
    ulong start,
    ulong width,
    ulong indent,
    bool *wrapped) {
  char *_tmp = null;

  string_new_copy_wrap(&_tmp, other, start, width, indent, wrapped);
  string_renew_append(p_this, _tmp);
  string_delete(&_tmp);
}

void string_new_f_wrap(
    char **p_this,
    ulong start,
    ulong width,
    ulong indent,
    bool *wrapped,
    char *fmt,
    ...) {
  va_list ap1;
  va_list ap2;

  va_start(ap1, fmt);
  va_start(ap2, fmt);
  string_new_vf(p_this, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);

  string_renew_wrap(p_this, start, width, indent, wrapped);
}

void string_renew_append_f_wrap(
    char **p_this,
    ulong start,
    ulong width,
    ulong indent,
    bool *wrapped,
    char *fmt,
    ...) {
  char *_s = null;
  va_list ap1;
  va_list ap2;

  va_start(ap1, fmt);
  va_start(ap2, fmt);
  string_new_vf(&_s, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);

  string_renew_wrap(&_s, start, width, indent, wrapped);
  string_renew_append(p_this, _s);
  string_delete(&_s);
}

char *string_copy(char *this, char *other, ulong max_len) {
  char *s;
  char *d;
  char *e;

  for (s = this, e = s + max_len, d = other; s < e && *s != '\0'; s++, d++) {
    *d = *s;
  }

  if (*s != '\0') {
    throw("max_len %lu but string length %lu", max_len, string_length(this));
  }

  *d = '\0';

  return d;
}

ulong string_f_get_max_len(char *fmt, ...) {
  ulong max_len;
  va_list ap;

  va_start(ap, fmt);
  max_len = string_vf_get_max_len(fmt, ap);
  va_end(ap);

  return max_len;
}

ulong string_vf_get_max_len(char *fmt, va_list ap) {
  bool done_left;
  bool done_sign;
  bool done_space;
  bool done_zero;
  bool done_hash;
  bool finished_flags;
  bool short_integer;
  bool long_integer;
  bool long_double;
  bool negative;
  char c;
  char *p;
  int min_exp;
  int max_exp;
  int precision;
  ulong min_width;
  ulong max_len;
  ulong width = 0;
  double d;
  long double ld;

  max_len = 0;

  for (p = fmt; (c = *p) != '\0'; p++) {
    min_width = 0;

    if (c == '%') {
      if ((c = *++p) == '%') {
        width = 1;
      } else {
        done_left = false;
        done_sign = false;
        done_space = false;
        done_zero = false;
        done_hash = false;
        finished_flags = false;

        for (;;) {
          switch (c) {
            case '-':
              if (done_left) {
                throw("'-' was already specified in format string");
              }

              done_left = true;
              break;

            case '+':
              if (done_sign) {
                throw("'+' was already specified in format string");
              }

              if (done_space) {
                throw("Can't specify both '+' and ' ' in format string");
              }

              done_sign = true;
              break;

            case ' ':
              if (done_space) {
                throw("' ' was already specified in format string");
              }

              if (done_sign) {
                throw("Can't specify both '+' and ' ' in format string");
              }

              done_space = true;
              break;

            case '0':
              if (done_zero) {
                throw("'0' was already specified in format string");
              }

              done_zero = true;
              break;

            case '#':
              if (done_hash) {
                throw("'#' was already specified in format string");
              }

              done_hash = true;
              break;

            default:
              finished_flags = true;
              break;
          }

          if (finished_flags) {
            break;
          }

          c = *++p;
        }

        if (c == '*') {
          min_width = va_arg(ap, int);
          c = *++p;
        } else if (isdigit(c)) {
          min_width = 0;

          while (isdigit(c)) {
            min_width *= 10;
            min_width += c - '0';
            c = *++p;
          }
        }

        if (c == '.') {
          c = *++p;
        }

        precision = -1;

        if (c == '*') {
          precision = va_arg(ap, int);
          c = *++p;
        } else if (isdigit(c)) {
          precision = 0;

          while (isdigit(c)) {
            precision *= 10;
            precision += c - '0';
            c = *++p;
          }
        }

        short_integer = false;
        long_integer = false;
        long_double = false;

        if (c == 'h') {
          short_integer = true;
          c = *++p;
        } else if (c == 'l') {
          long_integer = true;
          c = *++p;
        } else if (c == 'L') {
          long_double = true;
          c = *++p;
        }

        switch (c) {
          case 'd':
          case 'i':
          case 'u':
            if (long_double) {
              throw("Format 'L%c' is invalid", c);
            }

            if (short_integer) {
              width = 6;
              va_arg(ap, int);
            } else if (long_integer) {
              width = 21;
              va_arg(ap, long);
            } else {
              width = 11;
              va_arg(ap, int);
            }

            break;

          case 'o':
            if (long_double) {
              throw("Format 'L%c' is invalid", c);
            }

            if (short_integer) {
              width = 7;
              va_arg(ap, int);
            } else if (long_integer) {
              width = 23;
              va_arg(ap, long);
            } else {
              width = 12;
              va_arg(ap, int);
            }

            if (done_hash) {
              width++;
            }

            break;

          case 'x':
          case 'X':
            if (long_double) {
              throw("Format 'L%c' is invalid", c);
            }

            if (short_integer) {
              width = 4;
              va_arg(ap, int);
            } else if (long_integer) {
              width = 16;
              va_arg(ap, long);
            } else {
              width = 8;
              va_arg(ap, int);
            }

            if (done_hash) {
              width += 2;
            }

            break;

          case 'c':
            width = 1;
            va_arg(ap, int);
            break;

          case 's':
            width = string_length(va_arg(ap, char *));

            if (precision >= 0 && width > precision) {
              width = precision;
            }

            break;

          case 'f':
            if (short_integer) {
              throw("Format 'h%c' is invalid", c);
            }

            if (long_integer) {
              throw("Format 'l%c' is invalid", c);
            }

            if (precision < 0) {
              precision = 6;
            }

            if (long_double) {
              ld = va_arg(ap, long double);
              max_exp = max_exponent_(ld);
              width = precision + 2 + (max_exp > 0 ? max_exp + 1 : 1);
            } else {
              d = va_arg(ap, double);
              max_exp = max_exponent_(d);
              width = precision + 2 + (max_exp > 0 ? max_exp + 1 : 1);
            }

            break;

          case 'e':
          case 'E':
            if (precision < 0) {
              precision = 6;
            }

            width = 9 + precision;

            if (long_double) {
              va_arg(ap, long double);
            } else {
              va_arg(ap, double);
            }

            break;

          case 'g':
          case 'G':
            if (precision < 0) {
              precision = 6;
            }

            if (long_double) {
              ld = va_arg(ap, long double);
              min_exp = min_exponent_(ld);
              max_exp = max_exponent_(ld);
              negative = ld < 0;
            } else {
              d = va_arg(ap, double);
              min_exp = min_exponent_(d);
              max_exp = max_exponent_(d);
              negative = d < 0;
            }

            if (min_exp < -4 || max_exp >= precision) {
              width = 9 + precision;
            } else {
              width = precision + 2 + (max_exp > 0 ? max_exp + 1 : 1)
                + (min_exp < -2 ? -2 - min_exp : 0);
            }

            if (negative) {
              width++;
            }

            break;

          case 'p':
            width = 32;
            va_arg(ap, void *);
            break;

          case 'n':
            width = 0;
            min_width = 0;
            va_arg(ap, int *);
            break;

          default:
            if (isprint(c)) {
              throw("Character '%c' at index %lu of format string '%s' "
                  "is invalid",
                  c,
                  p - fmt,
                  fmt);
            } else {
              throw("Character 0x%02u at index %lu of format string '%s' "
                  "is invalid",
                  c,
                  p - fmt,
                  fmt);
            }
        }
      }

      if (width < min_width) {
        width = min_width;
      }

      max_len += width;
    } else {
      max_len++;
    }
  }

  return max_len;
}

void string_vf_print_into(char *this, char *fmt, va_list ap, ulong max_len) {
  ulong len_done;

  if ((len_done = vsprintf(this, fmt, ap)) > max_len) {
    throw("Length asked to be printed %lu; exceeded allocated %lu.",
        len_done,
        max_len);
  }
}


static int min_exponent_(double d) {
  if (d == 0) {
    return 0;
  }

  return (int)floor(log10(fabs(d)) - 1e-14);
}

static int max_exponent_(double d) {
  if (d == 0) {
    return 0;
  }

  return (int)floor(log10(fabs(d)) + 1e-14);
}

static void path_to_directory_filename_(
    char *path,
    char **directory,
    char **filename) {
  long l;

  l = string_last_index_of(path, '/');

  if (l < 0) {
    if (directory != null) {
      string_new_empty(directory);
    }

    if (filename != null) {
      string_new_copy(filename, path);
    }
  } else {
    if (directory != null) {
      string_new_truncate(directory, path, maximum(l, 1));
    }

    if (filename != null) {
      string_new_copy(filename, path + l + 1);
    }
  }

  if (directory != null) {
    string_renew_standardize_directory(directory);
  }
}

static void new_escape_(char **p_this, char *other, bool for_logging) {
  char c;
  char *p;
  char *q;
  ulong max_len;

  max_len = 4 * string_length(other);

  new(p_this, max_len + 1);

  for (p = *p_this, q = other; (c = *q) != '\0'; q++) {
    switch(c) {
      case '\a':
        *p++ = '\\';
        *p++ = 'a';
        break;
      case '\b':
        *p++ = '\\';
        *p++ = 'b';
        break;
      case '\f':
        *p++ = '\\';
        *p++ = 'f';
        break;
      case '\r':
        *p++ = '\\';
        *p++ = 'r';
        break;
      case '\t':
        *p++ = '\\';
        *p++ = 't';
        break;
      case '\v':
        *p++ = '\\';
        *p++ = 'v';
        break;
        break;
      case '\\':
      case '\n':
        if (!for_logging) {
          *p++ = '\\';
          *p++ = c == '\n' ? 'n' : '\\';
          break;
        }
      default:
        if (isprint(c) || (for_logging && (c == '\n' || c == '\\'))) {
          *p++ = c;
        } else {
          sprintf(p, "\\%03o", (byte)c);
          p += 4;
        }

        break;
    }
  }

  *p = '\0';

  string_renew_trim_length(p_this, max_len);
}
