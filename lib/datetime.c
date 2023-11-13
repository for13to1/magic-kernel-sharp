#include "datetime.h"
#include <time.h>

void string_new_datetime_current(
    char **p_this,
    bool utc,
    char *date_sep,
    char *t_sep,
    char *time_sep,
    char *decimal_sep,
    bool do_date,
    bool do_time,
    bool do_minutes,
    bool do_seconds,
    byte decimal_places) {
  struct timeval tv;

  if (gettimeofday(&tv, null) != 0) {
    throw("Could not get time");
  }

  string_new_datetime_from_timeval(
      p_this,
      &tv,
      utc,
      date_sep,
      t_sep,
      time_sep,
      decimal_sep,
      do_date,
      do_time,
      do_minutes,
      do_seconds,
      decimal_places);
}

void string_new_datetime_from_timeval(
    char **p_this,
    struct timeval *tv,
    bool utc,
    char *date_sep,
    char *t_sep,
    char *time_sep,
    char *decimal_sep,
    bool do_date,
    bool do_time,
    bool do_minutes,
    bool do_seconds,
    byte decimal_places) {
  byte d;
  uint f;
  struct tm t;

  if (decimal_places > 6) {
    throw("Decimal places (%u) cannot be more than 6", decimal_places);
  }

  t = time_t_to_tm(&tv->tv_sec, utc);

  string_new_empty(p_this);

  if (do_date) {
    if (date_sep == null) {
      throw("Cannot have null date_sep when date specified");
    }

    string_renew_append_f(
        p_this,
        "%04d%s%02d%s%02d",
        1900 + t.tm_year,
        date_sep,
        1 + t.tm_mon,
        date_sep,
        t.tm_mday);

    if (do_time) {
      string_renew_append_f(p_this, "%s", t_sep);
    }
  }

  if (do_time) {
    string_renew_append_f(p_this, "%02d", t.tm_hour);

    if (do_minutes) {
      if (time_sep == null) {
        throw("Cannot have null time_sep when minutes specified");
      }

      string_renew_append_f(p_this, "%s%02d", time_sep, t.tm_min);

      if (do_seconds) {
        string_renew_append_f(p_this, "%s%02d", time_sep, t.tm_sec);

        if (decimal_places != 0) {
          if (decimal_sep == null) {
            throw("Cannot have null decimal_sep with decimal places");
          }

          for (d = 0, f = 1; d < 6 - decimal_places; d++, f *= 10) {
          }

          string_renew_append_f(
              p_this,
              "%s%0*d",
              decimal_sep,
              decimal_places,
              (int)tv->tv_usec / f);
        }
      } else if (decimal_places != 0) {
        throw("Can't specify decimal places without seconds");
      }
    } else if (do_seconds || decimal_places != 0) {
      throw("Can't specify seconds or decimal places without minutes");
    }
  } else if (do_minutes || do_seconds || decimal_places != 0) {
    throw("Can't specify minutes, seconds or decimal places without time");
  }
}

struct tm time_t_to_tm(time_t *timer, bool utc) {
  static Lock lock__ = LOCK_INITIALIZER;

  struct tm t;

  lock(&lock__);

  if (utc) {
    t = *gmtime(timer);
  } else {
    t = *localtime(timer);
  }

  unlock(&lock__);

  return t;
}
