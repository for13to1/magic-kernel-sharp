#define LOG_C
#include "log.h"
#include <stdarg.h>

bool g_LOG_on_ = true;
bool g_LOG_fl_on_ = true;
char *g_LOG_file_ = null;
int g_LOG_line_ = -1;

static bool d_;
static bool escape_ = true;
static bool newlines_ = true;
static bool separators_ = true;
static bool throw_ = false;
static bool timestamps_ = true;
static bool utc_ = false;
static char *_last_file_ = null;
static int last_line_ = -2;
static long last_us_ = -1;
static Lock lock_r_ = LOCK_INITIALIZER;
static Lock lock_d_ = LOCK_INITIALIZER;

static void v_(FILE *f, byte i, char *fmt, va_list ap1, va_list ap2);

static void lock_(void);
static void unlock_(void);
static void unlock_d_(void);

void alog(FILE *f, byte i, char *fmt, ...) {
  va_list ap1;
  va_list ap2;

  d_ = false;
  va_start(ap1, fmt);
  va_start(ap2, fmt);
  v_(f, i, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);
}

void olog(byte i, char *fmt, ...) {
  va_list ap1;
  va_list ap2;

  d_ = false;
  va_start(ap1, fmt);
  va_start(ap2, fmt);
  v_(stdout, i, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);
}

void elog(byte i, char *fmt, ...) {
  va_list ap1;
  va_list ap2;

  d_ = false;
  va_start(ap1, fmt);
  va_start(ap2, fmt);
  v_(stderr, i, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);
}

void tlog(char *fmt, ...) {
  va_list ap1;
  va_list ap2;

  d_ = false;
  throw_ = true;
  va_start(ap1, fmt);
  va_start(ap2, fmt);
  v_(stderr, 0, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);
}

void dlogI(byte i, char *fmt, ...) {
  va_list ap1;
  va_list ap2;

  if (!g_LOG_on_) {
    unlock_d_();

    return;
  }

  d_ = true;
  va_start(ap1, fmt);
  va_start(ap2, fmt);
  v_(stderr, i, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);
  unlock_d_();
}

bool log_timestamps(bool b) {
  bool current;

  lock_();
  current = timestamps_;
  timestamps_ = b;
  unlock_();

  return current;
}

bool log_utc(bool b) {
  bool current;

  lock_();
  current = utc_;
  utc_ = b;
  unlock_();

  return current;
}

bool log_separators(bool b) {
  bool current;

  lock_();
  current = separators_;
  separators_ = b;
  unlock_();

  return current;
}

bool log_newlines(bool b) {
  bool current;

  lock_();
  current = newlines_;
  newlines_ = b;
  unlock_();

  return current;
}

bool log_escape(bool b) {
  bool current;

  lock_();
  current = escape_;
  escape_ = b;
  unlock_();

  return current;
}

void log_raw(bool b) {
  log_timestamps(!b);
  log_separators(!b);
  log_newlines(!b);
  log_escape(!b);
}

void ologn(byte i) {
  olog(i, "\n");
}

void log_lock_dI(void) {
  lock(&lock_d_);
}


static void lock_(void) {
  lock(&lock_r_);
}

static void unlock_(void) {
  unlock(&lock_r_);
}

static void unlock_d_(void) {
  unlock(&lock_d_);
}

static void v_(FILE *f, byte i, char *fmt, va_list ap1, va_list ap2) {
  bool e;
  char *_s = null;
  char *_s2 = null;
  char *_since = null;
  char *_time = null;
  char *_today = null;
  long l1;
  long l2;

  if (f == null) {
    return;
  }

  lock_();
  e = f == stderr;
  setbuf(f, null);

  string_new_empty(&_s);

  if (i > 0) {
    string_renew_indent(&_s, i);
  }

  if (!throw_ && (e || timestamps_)) {
    if (last_us_ < 0) {
      string_new_date_today(&_today, utc_);
      string_renew_prepend_f(
          &_s,
          "[ %s%s ] ",
          _today,
          utc_ ? "Z" : "");
      string_delete(&_today);
      last_us_ = unixtime_current_us();
    } else {
      string_new_time_since(&_since, &last_us_);
      string_new_datetime_current(
          &_time,
          utc_,
          null,
          null,
          ":",
          "!",
          false,
          true,
          true,
          false,
          0);
      string_renew_prepend_f(
          &_s,
          "[%s%s +%s] ",
          _time,
          utc_ ? "Z" : "",
          _since);
      string_delete(&_time);
      string_delete(&_since);
    }
  }

  fprintf(f, "%s", _s);
  string_delete(&_s);
  string_new_vf(&_s, fmt, ap1, ap2);

  if (e || separators_) {
    string_renew_add_separators(&_s);
  }

  if (e || escape_) {
    string_renew_escape_for_logging(&_s);
  }

  if (d_ && g_LOG_fl_on_) {
    if (last_line_ != g_LOG_line_ || !string_equals(g_LOG_file_, _last_file_)) {
      l1 = string_last_index_of(g_LOG_file_, '/');

      if (l1 < 0) {
        throw("Couldn't find '/' in file string");
      }

      l2 = string_last_index_of(g_LOG_file_, '.');

      if (l2 < 0) {
        throw("Couldn't find '.' in file string");
      }

      string_new_substring(&_s2, g_LOG_file_, l1 + 1, l2 - l1 - 1);
      string_renew_append_f(&_s, "   (%s:%d)", _s2, g_LOG_line_);
      string_delete(&_s2);

      string_delete(&_last_file_);
      string_new_copy(&_last_file_, g_LOG_file_);
      last_line_ = g_LOG_line_;
    }
  }

  fprintf(f, "%s", _s);
  string_delete(&_s);

  if (e || newlines_) {
    fprintf(f, "\n");
  }

  unlock_();
}
