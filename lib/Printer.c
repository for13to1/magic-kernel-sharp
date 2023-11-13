#include "Printer.h"

#define MAX_INDENT_ 99

static void do_(Printer *this, char **ps);


void printer_new(Printer **p_this, char **ps, byte indent) {
  Printer *this;

  this = newo(p_this);

  this->ps = ps;
  this->indent = indent;
  new(&this->_headings, this->maxHeadings = 1);
}

void printer_delete(Printer **p_this) {
  char *_s = null;
  ulong u;
  Printer *this;

  if ((this = *p_this) == null) {
    return;
  }

  if (this->numHeadings != 0) {
    string_new_f(
        &_s,
        "Printer destructor called with %lu heading%s still not finished: ",
        this->numHeadings,
        plural_s(this->numHeadings));

    for (u = 0; u < this->numHeadings; u++) {
      if (u > 0) {
        string_renew_append_f(&_s, ", ");
      }

      string_renew_append_f(&_s, "'%s'", this->_headings[u]);
    }

    throw(_s);
  }

  delete(&this->_headings, this->maxHeadings, string_delete);
  deleteo(p_this);
}

void sprint(Printer *this, char *fmt, ...) {
  char *_s = null;
  va_list ap1;
  va_list ap2;

  if (this->indent > MAX_INDENT_) {
    throw("Aborting print: indent level got to %u, which is crazy, and "
        "probably broken",
        this->indent);
  }

  if (string_contains_chars(fmt, "[/:]")) {
    throw("Do not use '[', '/', ':', or ']' in sprint heading strings");
  }

  if (this->numHeadings == this->maxHeadings) {
    renewup(&this->_headings,
        this->maxHeadings,
        2 * this->maxHeadings);
    this->maxHeadings *= 2;
  }

  va_start(ap1, fmt);
  va_start(ap2, fmt);
  string_new_vf(
      &this->_headings[this->numHeadings],
      fmt,
      ap1,
      ap2);
  va_end(ap1);
  va_end(ap2);

  string_new_f(&_s, "[%s]", this->_headings[this->numHeadings++]);
  do_(this, &_s);
  this->indent++;
  string_delete(&_s);
}

void fprint(Printer *this) {
  char *_s = null;

  if (this->numHeadings == 0) {
    throw("fprint() called with no sprint() headings available");
  }

  this->indent--;
  string_new_f(&_s, "[/%s]", this->_headings[--this->numHeadings]);
  string_delete(&this->_headings[this->numHeadings]);
  do_(this, &_s);
  string_delete(&_s);
}

void rprint(Printer *this, char *fmt, ...) {
  char *_s = null;
  va_list ap1;
  va_list ap2;

  va_start(ap1, fmt);
  va_start(ap2, fmt);
  string_new_vf(&_s, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);

  do_(this, &_s);
  string_delete(&_s);
}

void pointer_print(void *this, Printer *p) {
  rprint(p, "Pointer: %p", this);
}


static void do_(Printer *this, char **ps) {
  string_renew_indent(ps, this->indent);
  string_renew_add_separators(ps);

  if (this->ps == null) {
    log_lock_dI();
    dlogI(0, "%s", *ps);
  } else {
    string_renew_append(ps, "\n");

    if (*this->ps == null) {
      donate(ps, this->ps);
    } else {
      string_renew_append(this->ps, *ps);
    }
  }
}
