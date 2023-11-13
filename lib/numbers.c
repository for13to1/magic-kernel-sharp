#include "numbers.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

static void run_off_(char **pendp);

static bool try_parse_ulong_(ulong *u, char *s, byte base);

static bool try_parse_ulong_decimal_(ulong *u, char *s);


byte parse_bool(char *s) {
  bool result;

  if (!try_parse_bool(&result, s)) {
    throw("Couldn't parse '%s' as a bool", s);
  }

  return result;
}

byte parse_byte(char *s) {
  byte result;

  if (!try_parse_byte(&result, s)) {
    throw("Couldn't parse '%s' as a byte", s);
  }

  return result;
}

byte parse_byte_decimal(char *s) {
  byte result;

  if (!try_parse_byte_decimal(&result, s)) {
    throw("Couldn't parse '%s' as a byte in decimal format", s);
  }

  return result;
}

sbyte parse_sbyte(char *s) {
  sbyte result;

  if (!try_parse_sbyte(&result, s)) {
    throw("Couldn't parse '%s' as an sbyte", s);
  }

  return result;
}

ushort parse_ushort(char *s) {
  ushort result;

  if (!try_parse_ushort(&result, s)) {
    throw("Couldn't parse '%s' as a ushort", s);
  }

  return result;
}

short parse_short(char *s) {
  short result;

  if (!try_parse_short(&result, s)) {
    throw("Couldn't parse '%s' as a short", s);
  }

  return result;
}

uint parse_uint(char *s) {
  uint result;

  if (!try_parse_uint(&result, s)) {
    throw("Couldn't parse '%s' as a uint", s);
  }

  return result;
}

int parse_int(char *s) {
  int result;

  if (!try_parse_int(&result, s)) {
    throw("Couldn't parse '%s' as an int", s);
  }

  return result;
}

ulong parse_ulong(char *s) {
  ulong result;

  if (!try_parse_ulong(&result, s)) {
    throw("Couldn't parse '%s' as a ulong", s);
  }

  return result;
}

long parse_long(char *s) {
  long result;

  if (!try_parse_long(&result, s)) {
    throw("Couldn't parse '%s' as a long", s);
  }

  return result;
}

paw parse_paw(char *s) {
  paw result;


  if (!try_parse_paw(&result, s)) {
    throw("Couldn't parse '%s' as a paw", s);
  }

  return result;
}

double parse_double(char *s) {
  double result;

  if (!try_parse_double(&result, s)) {
    throw("Couldn't parse '%s' as a double", s);
  }

  return result;
}

bool try_parse_bool(bool *b, char *s) {
  if (string_equals(s, "true")) {
    *b = true;
    return true;
  }

  if (string_equals(s, "false")) {
    *b = false;
    return true;
  }

  if (string_equals(s, "True")) {
    *b = true;
    return true;
  }

  if (string_equals(s, "False")) {
    *b = false;
    return true;
  }

  if (string_equals(s, "1")) {
    *b = true;
    return true;
  }

  if (string_equals(s, "0")) {
    *b = false;
    return true;
  }

  return false;
}

bool try_parse_byte(byte *b, char *s) {
  ulong ul;

  if (!try_parse_ulong(&ul, s)) {
    return false;
  }

  if (ul > UCHAR_MAX) {
    return false;
  }

  *b = (byte)ul;
  return true;
}

bool try_parse_byte_decimal(byte *b, char *s) {
  ulong ul;

  if (!try_parse_ulong_decimal_(&ul, s)) {
    return false;
  }

  if (ul > UCHAR_MAX) {
    return false;
  }

  *b = (byte)ul;
  return true;
}

bool try_parse_sbyte(sbyte *b, char *s) {
  long l;

  if (!try_parse_long(&l, s)) {
    return false;
  }

  if (l < SCHAR_MIN || l > SCHAR_MAX) {
    return false;
  }

  *b = (sbyte)l;
  return true;
}

bool try_parse_ushort(ushort *u, char *s) {
  ulong ul;

  if (!try_parse_ulong(&ul, s)) {
    return false;
  }

  if (ul > USHRT_MAX) {
    return false;
  }

  *u = (ushort)ul;
  return true;
}

bool try_parse_short(short *h, char *s) {
  long l;

  if (!try_parse_long(&l, s)) {
    return false;
  }

  if (l > SHRT_MAX || l < SHRT_MIN) {
    return false;
  }

  *h = (short)l;
  return true;
}

bool try_parse_uint(uint *u, char *s) {
  ulong ul;

  if (!try_parse_ulong(&ul, s)) {
    return false;
  }

  if (ul > UINT_MAX) {
    return false;
  }

  *u = (uint)ul;
  return true;
}

bool try_parse_int(int *i, char *s) {
  long l;

  if (!try_parse_long(&l, s)) {
    return false;
  }

  if (l > INT_MAX || l < INT_MIN) {
    return false;
  }

  *i = (int)l;
  return true;
}

bool try_parse_ulong(ulong *u, char *s) {
  return try_parse_ulong_(u, s, 0);
}

bool try_parse_long(long *l, char *s) {
  char *endp;
  char *_t = null;
  long result;

  /* strtol will parse an empty string as zero, on Linux but not Unix. Don't
   * let it.
   */
  string_new_trim_whitespace(&_t, s);

  if (string_is_empty(_t)) {
    string_delete(&_t);
    return false;
  }

  errno = 0;
  result = strtol(_t, &endp, 0);
  run_off_(&endp);

  if (errno != 0 || *endp != '\0') {
    string_delete(&_t);
    return false;
  }

  string_delete(&_t);
  *l = result;
  return true;
}

bool try_parse_paw(paw *p, char *s) {
  double d;

  if (!try_parse_double(&d, s)) {
    return false;
  }

  *p = paw_make(d);

  return true;
}

bool try_parse_double(double *d, char *s) {
  char *endp;
  char *_t = null;
  double result;

  /* strtod will parse an empty string as zero. Don't let it.
   */
  string_new_trim_whitespace(&_t, s);

  if (string_is_empty(_t)) {
    string_delete(&_t);

    return false;
  }

  errno = 0;
  result = strtod(_t, &endp);
  run_off_(&endp);

  if (errno != 0 || *endp != '\0') {
    string_delete(&_t);

    return false;
  }

  string_delete(&_t);
  *d = result;

  return true;
}

void string_new_serialized_ulong(char **p_this, ulong u) {
  string_new_f(p_this, "0x%016lx", u);
}

void string_new_serialized_double(char **p_this, double d) {
  string_new_serialized_ulong(p_this, ulong_from_double(d));
}

sbyte short_compare(short *a, short *b, void *x) {
  short sa;
  short sb;

  sa = *a;
  sb = *b;

  return sa < sb ? -1 : sa > sb ? +1 : 0;
}

sbyte int_compare(int *a, int *b, void *x) {
  int ia;
  int ib;

  ia = *a;
  ib = *b;

  return ia < ib ? -1 : ia > ib ? +1 : 0;
}

sbyte uint_compare(uint *a, uint *b, void *x) {
  uint ua;
  uint ub;

  ua = *a;
  ub = *b;

  return ua < ub ? -1 : ua > ub ? +1 : 0;
}

sbyte long_compare(long *a, long *b, void *x) {
  long la;
  long lb;

  la = *a;
  lb = *b;

  return la < lb ? -1 : la > lb ? +1 : 0;
}

sbyte ulong_compare(ulong *a, ulong *b, void *x) {
  ulong ua;
  ulong ub;

  ua = *a;
  ub = *b;

  return ua < ub ? -1 : ua > ub ? +1 : 0;
}

sbyte double_compare(double *a, double *b, void *x) {
  double da;
  double db;

  da = *a;
  db = *b;

  return da < db ? -1 : da > db ? +1 : 0;
}

ulong gibibytes_to_bytes(double gibibytes) {
  double bytes;

  if (gibibytes < 0) {
    throw("Gibibytes (%.16g) cannot be negative", gibibytes);
  }

  bytes = gibibytes * 1024 * 1024 * 1024;

  if (bytes >= (double)ULONG_MAX + 0.499) {
    throw("%.16g GiB = %.16g bytes is too large to store in a ulong",
        gibibytes,
        bytes);
  }

  return (ulong)round(bytes);
}

double bytes_to_gibibytes(ulong bytes) {
  return (double)bytes / 1024 / 1024 / 1024;
}

byte ulong_digits(ulong u) {
  char s[32];

  sprintf(s, "%lu", u);
  return string_length(s);
}

void string_new_memory(char **p_this, ulong bytes) {
  byte u;
  char *units[] = {"B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB"};
  ulong k = 1024;
  ulong m1;
  ulong m2;
  ulong m3;

  for (u = 0, m1 = k, m2 = 1, m3 = 0; u < 6; u++, m3 = m2, m2 = m1, m1 *= k) {
    if (bytes < m1 - 512 * m3) {
      string_new_f(
          p_this,
          u == 0 || bytes / m2 >= 100 ? "%.*f %s" : "%.*g %s",
          u == 0 || bytes / m2 >= 100 ? 0 : 3,
          (double)bytes / m2,
          units[u]);

      return;
    }
  }

  string_new_f(p_this, "%.3g %s", (double)bytes / m2, units[u]);
}


static void run_off_(char **pendp) {
  char *endp;

  endp = *pendp;

  while (isspace(*endp)) {
    endp++;
  }

  *pendp = endp;
}

static bool try_parse_ulong_(ulong *u, char *s, byte base) {
  char c;
  char *p;
  char *endp;
  char *_t = null;
  ulong result;

  /* strtoul will parse an empty string as zero, on Linux but not Unix. Don't
   * let it.
   */
  string_new_trim_whitespace(&_t, s);

  if (string_is_empty(_t)) {
    string_delete(&_t);

    return false;
  }

  /* strtoul() accepts negative numbers. We don't want them.
   */
  for (p = _t; (c = *p) != '\0'; p++) {
    if (c == '-') {
      string_delete(&_t);

      return false;
    }
  }

  errno = 0;
  result = strtoul(_t, &endp, base);
  run_off_(&endp);

  if (errno != 0 || *endp != '\0') {
    string_delete(&_t);
    return false;
  }

  string_delete(&_t);
  *u = result;

  return true;
}

static bool try_parse_ulong_decimal_(ulong *u, char *s) {
  return try_parse_ulong_(u, s, 10);
}
