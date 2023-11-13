#include "unixtime.h"
#include <limits.h>

#define MS_ 1000UL
#define S_ (MS_ * MS_)
#define M_ (60 * S_)
#define H_ (60 * M_)
#define D_ (24 * H_)
#define Y_ (1461 * D_ /4)

static void interval_(char **p_this, ulong us, bool safe);

static void elapsed_(char **p_this,long now_us, long then_us, bool safe);

static void since_(char **p_this, long *last_us, bool safe);


long unixtime_current_us(void) {
  struct timeval tv;

  if (gettimeofday(&tv, null) != 0) {
    throw("Impossible: Could not get time");
  }

  return (long)tv.tv_sec * 1000000 + tv.tv_usec;
}

double unixtime_current(void) {
  return unixtime_current_us() * 0.000001;
}

void string_new_time_interval(char **p_this, ulong us) {
  interval_(p_this, us, false);
}

void string_new_time_interval_safe(char **p_this, ulong us) {
  interval_(p_this, us, true);
}

void string_new_time_elapsed(char **p_this, long now_us, long then_us) {
  elapsed_(p_this, now_us, then_us, false);
}

void string_new_time_elapsed_safe(char **p_this, long now_us, long then_us) {
  elapsed_(p_this, now_us, then_us, true);
}

void string_new_time_since(char **p_this, long *last_us) {
  since_(p_this, last_us, false);
}

void string_new_time_since_safe(char **p_this, long *last_us) {
  since_(p_this, last_us, true);
}

void deadline_progress(char **p_this, ulong count, ulong total_count, void *x) {
  char *_s = null;
  long deadline;
  long now;

  now = unixtime_current_us();
  deadline = *(long *)x;

  if (now >= deadline) {
    string_new_copy(p_this, "(no time left)");

    return;
  }

  string_new_time_elapsed(&_s, deadline, now);
  string_new_f(p_this, "(%s left)", _s);
  string_delete(&_s);
}


static void interval_(char **p_this, ulong us, bool safe) {
  ulong d;

  if ((d = us) > LONG_MAX) {
    throw("Time interval was negative: %ld us", (long)d);
  }

  if (d == 0) {
    string_new_copy(p_this, "00000");

    return;
  }

  if (d < MS_) {
    if (safe) {
      string_new_f(p_this, "%03luus", d);

      return;
    }

    string_new_f(p_this, "%03lu\xce\xbcs", d);

    return;
  }

  if (d < 10 * MS_) {
    string_new_f(p_this, "%lu.%lums", d / MS_, d % MS_ / (MS_ / 10));

    return;
  }

  if (d < S_) {
    string_new_f(p_this, "%03lums", d / MS_);

    return;
  }

  if (d < 10 * S_) {
    string_new_f(p_this, "%lu.%02lus", d / S_, d % S_ / (S_ / 100));

    return;
  }

  if (d < M_) {
    string_new_f(p_this, "%lu.%lus", d / S_, d % S_ / (S_ / 10));

    return;
  }

  if (d < 10 * M_) {
    string_new_f(p_this, "%lu.%02lum", d / M_, d % M_ / (M_ / 100));

    return;
  }

  if (d < H_) {
    string_new_f(p_this, "%lu.%lum", d / M_, d % M_ / (M_ / 10));

    return;
  }

  if (d < 10 * H_) {
    string_new_f(p_this, "%lu.%02luh", d / H_, d % H_ / (H_ / 100));

    return;
  }

  if (d < D_) {
    string_new_f(p_this, "%lu.%luh", d / H_, d % H_ / (H_ / 10));

    return;
  }

  if (d < 10 * D_) {
    string_new_f(p_this, "%lu.%02lud", d / D_, d % D_ / (D_ / 100));

    return;
  }

  if (d < 100 * D_) {
    string_new_f(p_this, "%lu.%lud", d / D_, d % D_ / (D_ / 10));

    return;
  }

  if (d < Y_) {
    string_new_f(p_this, "%04dd", d / D_);

    return;
  }

  if (d < 10 * Y_) {
    string_new_f(p_this, "%lu.%02luy", d / Y_, d % Y_ / (Y_ / 100));

    return;
  }

  if (d < 100 * Y_) {
    string_new_f(p_this, "%lu.%luy", d / Y_, d % Y_ / (Y_ / 10));

    return;
  }

  string_new_f(p_this, "%04luy", d / Y_);
}

static void elapsed_(char **p_this,
    long now_us,
    long then_us,
    bool safe) {
  if (now_us < then_us) {
    throw("Now (%ld us) is earlier than then (%ld us)",
        now_us,
        then_us);
  }

  interval_(p_this,
      now_us
      - (then_us <= 0 ? now_us : then_us),
      safe);
}

static void since_(char **p_this, long *last_us, bool safe) {
  long now_us;

  now_us = unixtime_current_us();
  elapsed_(p_this, now_us, *last_us, safe);
  *last_us = now_us;
}
