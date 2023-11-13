#include "Logger.h"

void logger_new(Logger **p_this, FILE *ansifile) {
  Logger *this;

  this = newo(p_this);

  this->ansifile = ansifile;
  new(&this->_messages, this->maxMessages = 1);
  lock_initialize(&this->lock);
}

void logger_delete(Logger **p_this) {
  char *_s = null;
  ulong u;
  Logger *this;

  if ((this = *p_this) == null) {
    return;
  }

  if (this->numMessages != 0) {
    string_new_f(
        &_s,
        "Logger destructor called with %lu message%s still not finished: ",
        this->numMessages,
        plural_s(this->numMessages));

    for (u = 0; u < this->numMessages; u++) {
      if (u > 0) {
        string_renew_append_f(&_s, ", ");
      }

      string_renew_append_f(&_s, "'%s'", this->_messages[u]);
    }

    throw(_s);
  }

  delete(&this->_messages, this->maxMessages, string_delete);

  deleteo(p_this);
}

void logger_print(Logger *this, Printer *p) {
  ulong u;

  sprint(p, "Logger");
  rprint(p, "Maximum number of messages: %lu", this->maxMessages);
  rprint(p, "Actual number of messages: %lu", this->numMessages);

  if (this->numMessages != 0) {
    sprint(p, "Messages");

    for (u = 0; u < this->numMessages; u++) {
      rprint(p, "[%lu] '%s'", u, this->_messages[u]);
    }

    fprint(p);
  }

  fprint(p);
}

void slog(Logger *this, char *fmt, ...) {
  char *message;
  va_list ap1;
  va_list ap2;

  if (this->ansifile == null) {
    return;
  }

  if (this->running) {
    throw("Cannot call slog() or elog() multi-threaded");
  }

  this->running = true;

  if (this->numMessages == this->maxMessages) {
    renewup(&this->_messages,
        this->maxMessages,
        2 * this->maxMessages);
    this->maxMessages *= 2;
  }

  va_start(ap1, fmt);
  va_start(ap2, fmt);
  string_new_vf(
      &this->_messages[this->numMessages],
      fmt,
      ap1,
      ap2);
  message = this->_messages[this->numMessages++];
  va_end(ap1);
  va_end(ap2);

  alog(this->ansifile, this->indent, "[%s]", message);
  this->indent++;
  this->running = false;
}

void flog(Logger *this) {
  if (this->ansifile == null) {
    return;
  }

  if (this->running) {
    throw("Cannot call slog() or elog() multi-threaded");
  }

  this->running = true;

  if (this->numMessages == 0) {
    throw("flog() called with no slog() messages available");
  }

  this->indent--;
  alog(this->ansifile,
      this->indent,
      "[/%s]",
      this->_messages[--this->numMessages]);
  string_delete(&this->_messages[this->numMessages]);
  this->running = false;
}

void rlog(Logger *this, char *fmt, ...) {
  char *_message = null;
  va_list ap1;
  va_list ap2;

  if (this->ansifile == null) {
    return;
  }

  if (this->running) {
    throw("Cannot call slog() or elog() multi-threaded with rlog()");
  }

  va_start(ap1, fmt);
  va_start(ap2, fmt);
  string_new_vf(&_message, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);

  alog(this->ansifile, this->indent, "%s", _message);
  string_delete(&_message);
}
