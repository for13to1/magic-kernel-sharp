#define TEST_C
#include "test.h"
#include <math.h>

bool g_TEST_call_ = false;
bool g_TEST_death_;
bool g_TEST_didnt_die_;
bool g_TEST_assert_all_ = false;
char *g_TEST_macro_ = "[No macro defined!]";
char *g_TEST_file_ = "[No file defined!]";
char *g_TEST_name_ = "[No test name defined!]";
char *g_TEST_subname_ = "[No test subname defined!]";
int g_TEST_line_ = -1;
ulong g_TEST_num_failures_ = 0;

void test_fail(bool assert, char *fmt, ...) {
  char *_f = null;
  char *_s = null;
  va_list ap1;
  va_list ap2;

  g_TEST_num_failures_++;

  va_start(ap1, fmt);
  va_start(ap2, fmt);
  string_new_vf(&_s, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);

  string_new_f(
      &_f,
      "%s failed in test %s.%s (%s:%d)%s:\n%s",
      g_TEST_macro_,
      g_TEST_name_,
      g_TEST_subname_,
      g_TEST_file_,
      g_TEST_line_,
      g_TEST_assert_all_ && !assert ? " (ASSERT_ALL is on)" : "",
      _s);
  string_delete(&_s);

  if (g_TEST_assert_all_ || assert) {
    g_JPC_exception_separators_ = false;
    throw("%s", _f);
  }

  fprintf(stderr, "\n%s\n\n", _f);
  string_delete(&_f);
}

void test_false(bool test, bool assert) {
  if (test) {
    test_fail(false, "Expected false but got true");
  }
}

void test_true(bool test, bool assert) {
  if (!test) {
    test_fail(assert, "Expected true but got false");
  }
}

void test_uleq(ulong expected, ulong test, bool assert) {
  if (expected != test) {
    test_fail(assert, "Expected %lu but got %lu", expected, test);
  }
}

void test_ulne(ulong expected, ulong test, bool assert) {
  if (expected == test) {
    test_fail(assert, "Didn't expect to equal %lu", expected);
  }
}

void test_leq(ulong expected, ulong test, bool assert) {
  if (expected != test) {
    test_fail(assert, "Expected %ld but got %ld", expected, test);
  }
}

void test_deq(double expected, double test, bool assert) {
  if (expected != test) {
    test_fail(assert, "Expected %.16g but got %.16g", expected, test);
  }
}

void test_paweq(paw expected, paw test, bool assert) {
  if (!paw_equals(expected, test)) {
    test_fail(assert,
        "Expected %.3g but got %.3g",
        double_from_paw(expected),
        double_from_paw(test));
  }
}

void test_peq(void *expected, void *test, bool assert) {
  if (expected != test) {
    test_fail(assert, "Expected %p but got %p", expected, test);
  }
}

void test_near(char *expected_code,
    double expected,
    char *actual_code,
    double actual,
    char *tolerance_code,
    double tolerance,
    bool assert) {
  if (fabs(actual - expected) > tolerance) {
    test_fail(assert,
        "The difference between %.16g and %.16g is %.16g, "
        "which exceeds %.16g, where\n"
        "%s evaluates to %.16g,\n"
        "%s evaluates to %.16g, and\n"
        "%s evaluates to %.16g.",
        expected,
        actual,
        fabs(actual - expected),
        tolerance,
        expected_code,
        expected,
        actual_code,
        actual,
        tolerance_code,
        tolerance);
  }
}

void test_streq(char *expected, char *test, bool assert) {
  bool multiline;
  char *sq;
  char *eq;
  char *p;
  char *q;

  if (!string_equals(expected, test)) {
    multiline = string_contains_char(expected, '\n')
      || string_contains_char(test, '\n');
    sq = multiline ? "\"\"\"\n" : "'";
    eq = multiline ? "\"\"\"" : "'";

    for (p = expected, q = test; *p == *q; p++, q++) {
    }

    test_fail(assert,
        "Expected %s%s%s but got %s%s%s, "
        "which first differ (0x%02x v 0x%02x) at %s%s%s versus %s%s%s",
        sq, expected, eq,
        sq, test, eq,
        *p, *q,
        sq, p, eq,
        sq, q, eq);
  }
}

void test_strneq(char *expected, char *test, bool assert) {
  if (string_equals(expected, test)) {
    test_fail(assert, "Expected string to not be '%s'", test);
  }
}

void test_null(void *p, bool assert) {
  if (p != null) {
    test_fail(assert, "Expected null pointer but got nonnull");
  }
}

void test_nonnull(void *p, bool assert) {
  if (p == null) {
    test_fail(assert, "Expected nonnull pointer but got null");
  }
}

void test_general(char *error, bool assert) {
  if (error != null) {
    test_fail(assert, error);
  }
}
