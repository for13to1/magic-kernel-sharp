#include "Progress.h"
#include <math.h>

#define FEEDBACK_MICROSECONDS_ 5000000
#define CHECK_MICROSECONDS_ 10

static Progress *new_(Progress **p_this, Logger *logger);

static void callback_(char **p_this, Progress *p);


void progress_new(Progress **p_this, Logger *logger, ulong total_count) {
  Progress *this;

  this = new_(p_this, logger);

  if (total_count == 0) {
    throw("Cannot have a total count of zero");
  }

  this->totalCount = total_count;
  string_new_empty(&this->_lastFeedback);
}

void progress_new_unbounded(
    Progress **p_this,
    struct Logger *logger,
    char *singular_string,
    char *plural_string) {
  Progress *this;

  this = new_(p_this, logger);

  string_new_copy(&this->_singularString, singular_string);
  string_new_copy(&this->_pluralString, plural_string);
}

void progress_delete(Progress **p_this) {
  char *_s = null;
  Progress *this;

  if ((this = *p_this) == null) {
    return;
  }

  if (this->totalCount > 0) {
    if (this->count != this->totalCount) {
      throw("Progress object being destroyed but only %lu of %lu "
          "operations were performed",
          this->count,
          this->totalCount);
    }
  } else {
    callback_(&_s, this);

    rlog(this->logger,
        "%lu %s in total%s.",
        this->count,
        this->count == 1 ? this->_singularString : this->_pluralString,
        _s);

    string_delete(&_s);
  }

  deletev(&this->_xs);
  deletev(&this->_callbacks);
  string_delete(&this->_lastFeedback);
  string_delete(&this->_pluralString);
  string_delete(&this->_singularString);

  deleteo(p_this);
}

void progress_print(Progress *this, Printer *p) {
  sprint(p, "Progress");
  rprint(p, "Total count: %lu", this->totalCount);
  rprint(p, "Current count per check: %lu", this->countPerCheck);
  rprint(p, "Current count: %lu", this->count);
  rprint(p, "Last check count: %lu", this->lastCheckCount);
  rprint(p, "Last check us: %ld", this->lastCheckUs);
  rprint(p, "Last feedback us: %ld", this->lastFeedbackUs);

  if (this->totalCount > 0) {
    rprint(p, "Last feedback: '%s'", this->_lastFeedback);
  } else {
    rprint(p, "Singular string: '%s'", this->_singularString);
    rprint(p, "Plural string: '%s'", this->_pluralString);
  }

  fprint(p);
}

void progress_add_callback(
    Progress *this,
    void (*callback)(char **p_this, ulong count, ulong total_count, void *x),
    void *x) {
  append_copy(&this->_callbacks, this->numCallbacks, callback);
  this->numCallbacks--;
  append_copy(&this->_xs, this->numCallbacks, x);
}

void progress_bump(Progress *this) {
  progress_bump_multi(this, 1);
}

void progress_bump_multi(Progress *this, ulong count) {
  byte digits;
  char *_feedback = null;
  char *_s = null;
  double percentage;
  long delta;
  long now;

  if (count == 0) {
    throw("Count cannot be zero");
  }

  if (this == null) {
    return;
  }

  lock(&this->lock);

  if (this->totalCount > 0 && this->count == this->totalCount) {
    throw("Total count of %lu operation%s was already reached!",
        this->count,
        plural_s(this->count));
  }

  this->count += count;

  if (this->count != this->totalCount
      && this->count - this->lastCheckCount >= this->countPerCheck) {
    this->lastCheckCount = this->count;
    now = unixtime_current_us();
    delta = now - this->lastCheckUs;
    this->lastCheckUs = now;

    if (delta < CHECK_MICROSECONDS_ / 2) {
      this->countPerCheck *= 2;
    } else if (this->countPerCheck > 1 && delta > CHECK_MICROSECONDS_ * 2) {
      this->countPerCheck = maximum(1,
          (ulong)floor(this->countPerCheck * CHECK_MICROSECONDS_ / delta));
    }

    if (now - this->lastFeedbackUs >= FEEDBACK_MICROSECONDS_) {
      callback_(&_s, this);

      if (this->totalCount > 0) {
        percentage = this->count * 100.0 / this->totalCount;
        digits = 0;

        do {
          string_delete(&_feedback);
          string_new_f(&_feedback, "%.*f%% done", digits, percentage);
          digits++;
        } while (string_equals(_feedback, this->_lastFeedback)
            && !string_equals(_feedback, "100% done"));

        if (!string_equals(_feedback, "100% done")) {
          string_delete(&this->_lastFeedback);
          string_new_copy(&this->_lastFeedback, _feedback);
          string_renew_append_f(&_feedback, "%s ...",  _s);
          rlog(this->logger, "%s", _feedback);
        }

        string_delete(&_feedback);
      } else {
        rlog(this->logger,
            "%lu %s%s ...",
            this->count,
            this->count == 1 ? this->_singularString : this->_pluralString,
            _s);
      }

      string_delete(&_s);
      this->lastFeedbackUs = now;
    }
  }

  unlock(&this->lock);
}


static Progress *new_(Progress **p_this, Logger *logger) {
  Progress *this;

  this = newo(p_this);

  this->logger = logger;
  this->countPerCheck = 1;
  new(&this->_callbacks, 0);
  new(&this->_xs, 0);
  lock_initialize(&this->lock);
  this->lastCheckUs = this->lastFeedbackUs = unixtime_current_us();

  return this;
}

static void callback_(char **p_this, Progress *p) {
  char *_s = null;
  ulong c;

  string_new_empty(p_this);

  for (c = 0; c < p->numCallbacks; c++) {
    p->_callbacks[c](&_s, p->count, p->totalCount, p->_xs[c]);

    if (_s != null) {
      string_renew_append_f(p_this, " %s", _s);
      string_delete(&_s);
    }
  }
}
