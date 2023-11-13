#include "TextFile.h"

#define ERROR_SEP_ '!'

static TextFile *new_(TextFile **p_this);

static void error_(char *filename, char *mode);

static char separator_from_type_(char *type);

static char try_separator_from_type_(char *type);


void text_file_new(TextFile **p_this, char *filename, char *mode) {
  if (!text_file_try_new(p_this, filename, mode)) {
    error_(filename, mode);
  }
}

void text_file_new_type(
    TextFile **p_this,
    char *filename,
    char *mode,
    char *filetype) {
  if (!text_file_try_new_type(p_this, filename, mode, filetype)) {
    error_(filename, mode);
  }
}

void text_file_new_memory(TextFile **p_this, char *filetype) {
  text_file_new_type(p_this, null, null, filetype);
}

bool text_file_try_new(TextFile **p_this, char *filename, char *mode) {
  bool b;
  char *_extension = null;

  string_new_extension_from_path(&_extension, filename);

  if (_extension == null) {
    throw("Could not infer a filetype extension from the filename '%s'",
        filename);
  }

  b = text_file_try_new_type(p_this, filename, mode, _extension);
  string_delete(&_extension);

  return b;
}

bool text_file_try_new_type(
    TextFile **p_this,
    char *filename,
    char *mode,
    char *filetype) {
  TextFile *this;

  this = new_(p_this);

  this->separator = separator_from_type_(filetype);

  if (!file_try_new(&this->_file, filename, mode)) {
    text_file_delete(p_this);

    return false;
  }

  return true;
}

void text_file_delete(TextFile **p_this) {
  TextFile *this;

  if ((this = *p_this) == null) {
    return;
  }

  string_delete(&this->_record);
  deletev(&this->_fields);

  file_delete(&this->_file);

  deleteo(p_this);
}

void text_file_print(TextFile *this, Printer *p) {
  char **fields;
  ulong num_fields;
  ulong u;
  ulong r;

  sprint(p, "TextFile");
  rprint(p, "Separator: 0x%02x", this->separator);

  sprint(p, "Records");
  text_file_rewind(this);
  r = 0;

  while (text_file_try_read(this, &num_fields, &fields)) {
    sprint(p, "Record %lu", r++);
    rprint(p, "Number of fields: %lu", num_fields);

    for (u = 0; u < num_fields; u++) {
      rprint(p, "Field %lu: '%s'", u, fields[u]);
    }

    fprint(p);
  }

  fprint(p);
  file_print(this->_file, p);
  fprint(p);
}

bool text_file_type_is_valid(char *filetype) {
  return try_separator_from_type_(filetype) != '!';
}

void text_file_write(TextFile *this, char *s) {
  char c;
  char *p;
  char *_t = null;

  if (this->currentFieldIndex > 0) {
    file_write_byte(this->_file, (byte)this->separator);
  }

  for (p = s; (c = *p) != '\0'; p++) {
    if (c == this->separator) {
      throw("Separator character '%c' found in field '%s' being written",
          c,
          s);
    }

    if (c == '\r') {
      string_new_escape(&_t, s);
      throw("Line terminator character '%c' found in field '%s' being written",
          c,
          _t);
    }

    if (c == '\n') {
      string_new_escape(&_t, s);
      throw("Line terminator character '\\n' found in field '%s' being written",
          _t);
    }

    file_write_byte(this->_file, (byte)c);
  }

  this->currentFieldIndex++;
}

void text_file_write_f(TextFile *this, char *fmt, ...) {
  char *_field = null;
  ulong len;
  va_list ap;

  va_start(ap, fmt);
  len = string_vf_get_max_len(fmt, ap);
  va_end(ap);

  new(&_field, len + 1);

  va_start(ap, fmt);
  string_vf_print_into(_field, fmt, ap, len);
  va_end(ap);

  text_file_write(this, _field);

  string_delete(&_field);
}

void text_file_write_bool(TextFile *this, bool b) {
  text_file_write(this, bool_string(b));
}

void text_file_write_ulong(TextFile *this, ulong u) {
  text_file_write_f(this, "%lu", u);
}

void text_file_write_serialized_ulong(TextFile *this, ulong u) {
  char *_s = null;

  string_new_serialized_ulong(&_s, u);
  text_file_write(this, _s);
  string_delete(&_s);
}

void text_file_write_long(TextFile *this, long l) {
  text_file_write_f(this, "%ld", l);
}

void text_file_write_paw(TextFile *this, paw p) {
  text_file_write_f(this, "%.4g", double_from_paw(p));
}

void text_file_write_double(TextFile *this, double d) {
  text_file_write_f(this, "%.16g", d);
}

void text_file_write_serialized_double(TextFile *this, double d) {
  char *_s = null;

  string_new_serialized_double(&_s, d);
  text_file_write(this, _s);
  string_delete(&_s);
}

void text_file_write_empty_field(TextFile *this) {
  text_file_write_f(this, "");
}

void text_file_end_record(TextFile *this) {
  file_write_byte(this->_file, (byte)'\n');
  this->currentFieldIndex = 0;
}

void text_file_write_record(TextFile *this, ulong num_fields, char **fields) {
  ulong f;

  for (f = 0; f < num_fields; f++) {
    text_file_write(this, fields[f]);
  }

  text_file_end_record(this);
}

void text_file_write_doubles_record(
    TextFile *this,
    ulong num_fields,
    double *fields) {
  ulong f;

  for (f = 0; f < num_fields; f++) {
    text_file_write_double(this, fields[f]);
  }

  text_file_end_record(this);
}

void text_file_write_empty_record(TextFile *this) {
  if (this->currentFieldIndex > 0) {
    throw("Cannot write empty record from the middle of a record");
  }

  text_file_end_record(this);
}

void text_file_write_single(TextFile *this, char *s) {
  text_file_write(this, s);
  text_file_end_record(this);
}

void text_file_write_single_ulong(TextFile *this, ulong u) {
  text_file_write_ulong(this, u);
  text_file_end_record(this);
}

void text_file_write_single_serialized_ulong(TextFile *this, ulong u) {
  text_file_write_serialized_ulong(this, u);
  text_file_end_record(this);
}

void text_file_write_single_long(TextFile *this, long l) {
  text_file_write_long(this, l);
  text_file_end_record(this);
}

void text_file_write_single_double(TextFile *this, double d) {
  text_file_write_double(this, d);
  text_file_end_record(this);
}

void text_file_write_single_serialized_double(TextFile *this, double d) {
  text_file_write_serialized_double(this, d);
  text_file_end_record(this);
}

void text_file_write_label(TextFile *this, char *label) {
  text_file_write_single(this, label);
}

void text_file_write_labeled(TextFile *this, char *label, char *s) {
  text_file_write(this, label);
  text_file_write(this, s);
  text_file_end_record(this);
}

void text_file_write_labeled_bool(TextFile *this, char *label, bool b) {
  text_file_write(this, label);
  text_file_write_bool(this, b);
  text_file_end_record(this);
}

void text_file_write_labeled_byte(TextFile *this, char *label, byte b) {
  text_file_write_labeled_ulong(this, label, b);
}

void text_file_write_labeled_ulong(TextFile *this, char *label, ulong u) {
  text_file_write(this, label);
  text_file_write_ulong(this, u);
  text_file_end_record(this);
}

ulong text_file_read(TextFile *this, char ***fields) {
  ulong num_fields;

  if (!text_file_try_read(this, &num_fields, fields)) {
    throw("Couldn't read record");
  }

  return num_fields;
}

bool text_file_try_read(TextFile *this, ulong *num_fields, char ***fields) {
  bool eof;
  byte read;
  char c;
  char sep;
  char *p;
  char *pe;
  char **f;
  char **fe;
  ulong max;
  ulong new_max;
  File *file;

  sep = this->separator;
  file = this->_file;

  max = this->maxNumRecordChars;

  p = this->_record;
  pe = p + max;

  eof = false;

  for (;;) {
    if (!file_try_read_byte(file, &read)) {
      *p = '\0';
      eof = true;

      break;
    }

    c = (char)read;

    if (sep != '\0' && c == '\0') {
      throw("Read invalid '\\0' in TextFile");
    }

    if (c == '\n') {
      if (this->justReadCR) {
        this->justReadCR = false;
        continue;
      }

      this->justReadCR = false;
      *p = '\0';
      break;
    }

    if (c == '\r') {
      this->justReadCR = true;
      *p = '\0';

      break;
    } else {
      this->justReadCR = false;
    }

    *p = c;

    if (++p == pe) {
      new_max = max << 1;
      renewup(&this->_record, max, new_max);
      this->maxNumRecordChars = new_max;
      p = this->_record + max;
      pe = this->_record + new_max;
      max = new_max;
    }
  }

  pe = p;
  p = this->_record;

  this->numRecordChars = pe - p + 1;

  max = this->maxNumFields;

  f = this->_fields;
  fe = f + max;

  while (p < pe) {
    *f = p;

    if (++f == fe) {
      new_max = max << 1;
      renewup(&this->_fields, max, new_max);
      this->maxNumFields = new_max;
      f = this->_fields + max;
      fe = this->_fields + new_max;
      max = new_max;
    }

    for (; p < pe; p++) {
      if (*p == sep) {
        *p++ = '\0';

        if (p == pe) {
          *f++ = p;
        }

        break;
      }
    }
  }

  this->numFields = *num_fields = f - this->_fields;
  *fields = this->_fields;

  for (; f < fe; f++) {
    *f = null;
  }

  return !eof || *num_fields > 0;
}

void text_file_read_copy(TextFile *this, ulong *num_fields, char ***p_fields) {
  if (!text_file_try_read_copy(this, num_fields, p_fields)) {
    throw("Couldn't read record");
  }
}

bool text_file_try_read_copy(
    TextFile *this,
    ulong *num_fields,
    char ***p_fields) {
  char **fields;
  char **ref;
  ulong f;

  if (!text_file_try_read(this, num_fields, &ref)) {
    return false;
  }

  fields = new(p_fields, *num_fields);

  for (f = 0; f < *num_fields; f++) {
    string_new_copy(&fields[f], ref[f]);
  }

  return true;
}

void text_file_read_discard(TextFile *this) {
  char **fields;

  text_file_read(this, &fields);
}

bool text_file_try_read_discard(TextFile *this) {
  char **fields;
  ulong num_fields;

  return text_file_try_read(this, &num_fields, &fields);
}

void text_file_read_empty_record(TextFile *this) {
  char **fields;
  ulong num_fields;

  num_fields = text_file_read(this, &fields);

  if (num_fields > 0) {
    throw("Record was not empty: it had %lu field%s",
        num_fields,
        plural_s(num_fields));
  }
}

char *text_file_read_single(TextFile *this) {
  char *s;

  if (!text_file_try_read_single(this, &s)) {
    throw("Could not read from file");
  }

  return s;
}

bool text_file_try_read_single(TextFile *this, char **s) {
  char **fields;
  ulong num_fields;

  if (!text_file_try_read(this, &num_fields, &fields)) {
    *s = null;
    return false;
  }

  if (num_fields == 0) {
    *s = "";
    return true;
  }

  if (num_fields == 1) {
    *s = fields[0];
    return true;
  }

  throw("Expected only a single field but read %lu fields", num_fields);

  return false;
}

ulong text_file_read_single_ulong(TextFile *this) {
  char *field;

  field = text_file_read_single(this);
  return parse_ulong(field);
}

double text_file_read_single_serialized_double(TextFile *this) {
  return double_from_ulong(text_file_read_single_ulong(this));
}

void text_file_read_label(TextFile *this, char *label) {
  char *read;

  read = text_file_read_single(this);

  if (!string_equals(read, label)) {
    throw("Expected to read label '%s', but read label '%s'", label, read);
  }
}

char *text_file_read_labeled(TextFile *this, char *label) {
  char *read;
  char **fields;
  ulong num_fields;

  num_fields = text_file_read(this, &fields);
  read = num_fields == 0 ? "" : fields[0];

  if (!string_equals(read, label)) {
    throw("Expected to read label '%s', but read label '%s'", label, read);
  }

  return num_fields < 2 ? "" : fields[1];
}

bool text_file_read_labeled_bool(TextFile *this, char *label) {
  return parse_bool(text_file_read_labeled(this, label));
}

byte text_file_read_labeled_byte(TextFile *this, char *label) {
  return parse_byte(text_file_read_labeled(this, label));
}

ulong text_file_read_labeled_ulong(TextFile *this, char *label) {
  return parse_ulong(text_file_read_labeled(this, label));
}

void text_file_rewind(TextFile *this) {
  file_rewind(this->_file);
}

char *text_file_get_filename(TextFile *this) {
  return file_get_filename(this->_file);
}

void string_new_text_file_contents(char **p_this, TextFile *tf) {
  string_new_file_contents(p_this, tf->_file);
}


static TextFile *new_(TextFile **p_this) {
  TextFile *this;

  this = newo(p_this);

  new(&this->_record, this->maxNumRecordChars = 16);
  new(&this->_fields, this->maxNumFields = 2);

  return this;
}

static void error_(char *filename, char *mode) {
  throw("Could not open file '%s' in mode '%s'", filename, mode);
}

static char separator_from_type_(char *type) {
  char separator;

  if (type == null) {
    throw("File type string was null");
  }

  separator = try_separator_from_type_(type);

  if (separator == ERROR_SEP_) {
    throw("Do not recognize filetype extension '%s'", type);
  }

  return separator;
}

static char try_separator_from_type_(char *type) {
  char separator;

  if (type == null) {
    return ERROR_SEP_;
  }

  if (string_equals(type, "csv")) {
    separator = ',';
  } else if (string_equals(type, "tsv")) {
    separator = '\t';
  } else if (string_equals(type, "hsv")) {
    separator = '\x01';
  } else if (string_equals(type, "nsv") || string_equals(type, "txt")) {
    separator = '\0';
  } else {
    separator = ERROR_SEP_;
  }

  return separator;
}
