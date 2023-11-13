#include "File.h"

static void new_vf_(File **p_this,
    char *mode,
    char *fmt,
    va_list ap1,
    va_list ap2);

static bool try_new_vf_(
    File **p_this,
    char *mode,
    char *fmt,
    va_list ap1,
    va_list ap2);


void file_new(File **p_this, char *filename, char *mode) {
  if (!file_try_new(p_this, filename, mode)) {
    throw("Could not open file '%s' in mode '%s'", filename, mode);
  }
}

bool file_try_new(
    File **p_this,
    char *filename,
    char *mode) {
  if (mode == null) {
    file_new_memory(p_this);

    return true;
  }

  return file_try_new_external(p_this, filename, mode);
}

void file_new_f(File **p_this, char *mode, char *fmt, ...) {
  va_list ap1;
  va_list ap2;

  va_start(ap1, fmt);
  va_start(ap2, fmt);
  new_vf_(p_this, mode, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);
}

bool file_try_new_f(File **p_this, char *mode, char *fmt, ...) {
  bool success;
  va_list ap1;
  va_list ap2;

  va_start(ap1, fmt);
  va_start(ap2, fmt);
  success = try_new_vf_(p_this, mode, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);

  return success;
}

void file_new_memory(File **p_this) {
  File *this;

  this = newo(p_this);

  memory_file_new(&this->_mf);
}

void file_new_external(File **p_this, char *filename, char *mode) {
  if (!file_try_new_external(p_this, filename, mode)) {
    throw("Could not open external file '%s' in mode '%s'", filename, mode);
  }
}

bool file_try_new_external(
    File **p_this,
    char *filename,
    char *mode) {
  File *this;

  this = newo(p_this);

  if (!external_file_try_new((ExternalFile **)&this->_ef, filename, mode)) {
    file_delete(p_this);

    return false;
  }

  return true;
}

void file_new_wrap_ansifile(File **p_this, FILE *ansifile, char *mode) {
  File *this;

  this = newo(p_this);

  external_file_new_wrap_ansifile((ExternalFile **)&this->_ef, ansifile, mode);
}

void file_new_memory_copy_from_filename(File **p_this, char *filename) {
  file_new_copy_from_filename(p_this, null, null, filename);
}

void file_new_memory_copy_external(File **p_this, struct ExternalFile *ef) {
  file_new_copy_external(p_this, null, null, ef);

}

void file_new_copy_from_filename(
    File **p_this,
    char *filename,
    char *mode,
    char *other_filename) {
  File *_other = null;

  file_new(&_other, other_filename, "r");
  file_new_copy(p_this, filename, mode, _other);
  file_delete(&_other);
}

void file_new_copy(File **p_this, char *filename, char *mode, File *other) {
  File *this;

  file_new(p_this, filename, mode);
  this = *p_this;

  file_copy_other(this, other);
}

void file_new_copy_external(
    File **p_this,
    char *filename,
    char *mode,
    struct ExternalFile *ef) {
  File *this;

  file_new(p_this, filename, mode);
  this = *p_this;

  file_copy_external(this, ef);
}

void file_new_memory_copy_string(File **p_this, char *s) {
  char c;
  char *p;
  File *this;

  file_new(p_this, null, null);
  this = *p_this;

  for (p = s; (c = *p) != '\0'; p++) {
    file_write_char(this, c);
  }
}

void file_delete(File **p_this) {
  File *this;

  if ((this = *p_this) == null) {
    return;
  }

  memory_file_delete(&this->_mf);
  external_file_delete((ExternalFile **)&this->_ef);

  deleteo(p_this);
}

void file_print(File *this, Printer *p) {
  sprint(p, "File");

  if (file_is_memory(this)) {
    rprint(p, "Memory file: true");
    rprint(p, "External file: false");
    memory_file_print(this->_mf, p);
  } else {
    rprint(p, "Memory file: false");
    rprint(p, "External file: true");
    external_file_print((ExternalFile *)this->_ef, p);
  }

  fprint(p);
}

void file_release(File *this) {
  if (this->_mf == null) {
    throw("Can only release memory files");
  }

  memory_file_release(this->_mf);
}

void file_reacquire(File *this) {
  if (this->_mf == null) {
    throw("Can only reacquire memory files");
  }

  memory_file_reacquire(this->_mf);
}

bool file_is_gzipped(File *this) {
  return file_is_external(this)
    && external_file_is_gzipped((ExternalFile *)this->_ef);
}

bool file_is_memory(File *this) {
  return this->_mf != null;
}

bool file_is_external(File *this) {
  return this->_ef != null;
}

char *file_get_filename(File *this) {
  return file_is_external(this) ? ((ExternalFile *)this->_ef)->_filename : null;
}

char *file_get_mode(File *this) {
  return file_is_external(this) ? ((ExternalFile *)this->_ef)->_mode : null;
}

void file_write_byte(File *this, byte b) {
  if (file_is_memory(this)) {
    memory_file_write_byte(this->_mf, b);
  } else {
    external_file_write_byte((ExternalFile *)this->_ef, b);
  }
}

void file_write_bytes(File *this, byte *bs, ulong num_bytes) {
  if (file_is_memory(this)) {
    memory_file_write_bytes(this->_mf, bs, num_bytes);
  } else {
    external_file_write_bytes((ExternalFile *)this->_ef, bs, num_bytes);
  }
}

void file_write_bytesm(File *this, byte **bm, ulong n0, ulong num_bytes) {
  ulong u;

  if (file_is_memory(this)) {
    memory_file_write_bytesm(this->_mf, bm, n0, num_bytes);
  } else {
    for (u = 0; u < n0; u++) {
      external_file_write_bytes((ExternalFile *)this->_ef, bm[u], num_bytes);
    }
  }
}

void file_write_char(File *this, char c) {
  file_write_byte(this, (byte)c);
}

void file_write_string(File *this, char *s) {
  char c;
  char *p;

  for (p = s; (c = *p) != '\0'; p++) {
    file_write_byte(this, (byte)c);
  }
}

byte file_read_byte(File *this) {
  if (file_is_memory(this)) {
    return memory_file_read_byte(this->_mf);
  } else {
    return external_file_read_byte((ExternalFile *)this->_ef);
  }
}

bool file_try_read_byte(File *this, byte *b) {
  if (file_is_memory(this)) {
    return memory_file_try_read_byte(this->_mf, b);
  } else {
    return external_file_try_read_byte((ExternalFile *)this->_ef, b);
  }
}

void file_read_bytes(File *this, byte *bs, ulong num_bytes) {
  if (file_is_memory(this)) {
    if (bs == null) {
      memory_file_set_position(
          this->_mf,
          memory_file_get_position(this->_mf) + num_bytes);
    } else {
      memory_file_read_bytes(this->_mf, bs, num_bytes);
    }
  } else {
    external_file_read_bytes((ExternalFile *)this->_ef, bs, num_bytes);
  }
}

void file_read_bytesm(File *this, byte **bm, ulong n0, ulong num_bytes) {
  ulong u;

  if (file_is_memory(this)) {
    memory_file_read_bytesm(this->_mf, bm, n0, num_bytes);
  } else {
    for (u = 0; u < n0; u++) {
      external_file_read_bytes((ExternalFile *)this->_ef, bm[u], num_bytes);
    }
  }
}

ulong file_get_position(File *this) {
  if (file_is_memory(this)) {
    return memory_file_get_position(this->_mf);
  } else {
    return external_file_get_position((ExternalFile *)this->_ef);
  }
}

void file_set_position(File *this, ulong position) {
  if (file_is_memory(this)) {
    memory_file_set_position(this->_mf, position);
  } else {
    external_file_set_position((ExternalFile *)this->_ef, position);
  }
}

void file_rewind(File *this) {
  if (file_is_memory(this)) {
    memory_file_rewind(this->_mf);
  } else {
    external_file_rewind((ExternalFile *)this->_ef);
  }
}

void file_fast_forward(File *this) {
  if (file_is_memory(this)) {
    memory_file_fast_forward(this->_mf);
  } else {
    external_file_fast_forward((ExternalFile *)this->_ef);
  }
}

void file_backup_one_byte(File *this) {
  ulong pos;

  if ((pos = file_get_position(this)) == 0) {
    throw("Cannot backup one byte when file is at position 0");
  }

  file_set_position(this, pos - 1);
}

ulong file_get_size(File *this) {
  if (file_is_memory(this)) {
    return memory_file_get_size(this->_mf);
  } else {
    return external_file_get_size((ExternalFile *)this->_ef);
  }
}

void file_flush(File *this) {
  if (file_is_external(this)) {
    external_file_flush((ExternalFile *)this->_ef);
  }
}

FILE *file_get_ansifile(File *this) {
  if (!file_is_external(this)) {
    throw("Cannot get ansifile for a memory file");
  }

  return external_file_get_ansifile((ExternalFile *)this->_ef);
}

bool file_contents_equals_blob(File *this, ulong len, byte *blob) {
  if (file_is_memory(this)) {
    return memory_file_contents_equals_blob(this->_mf, len, blob);
  } else {
    return external_file_contents_equals_blob(
        (ExternalFile *)this->_ef,
        len,
        blob);
  }
}

void file_assert_contents_equals_blob(File *this, ulong len, byte *blob) {
  if (file_is_memory(this)) {
    memory_file_assert_contents_equals_blob(this->_mf, len, blob);
  } else {
    external_file_assert_contents_equals_blob(
        (ExternalFile *)this->_ef,
        len,
        blob);
  }
}

bool file_contents_equals_string(File *this, char *s) {
  return file_contents_equals_blob(this, string_length(s), (byte *)s);
}

void file_assert_contents_equals_string(File *this, char *s) {
  file_assert_contents_equals_blob(this, string_length(s), (byte *)s);
}

void file_copy_other(File *this, File *other) {
  byte b;

  while (file_try_read_byte(other, &b)) {
    file_write_byte(this, b);
  }
}

void file_copy_external(File *this, struct ExternalFile *ef) {
  byte b;

  while (external_file_try_read_byte((ExternalFile *)ef, &b)) {
    file_write_byte(this, b);
  }
}

bool file_exists(char *filename) {
  return external_file_exists(filename);
}

bool file_exists_wait(char *filename, byte max_seconds) {
  return external_file_exists_wait(filename, max_seconds);
}

bool file_partial_exists(char *filename) {
  return external_file_partial_exists(filename);
}

bool file_either_exists(char *filename) {
  return external_file_either_exists(filename);
}

bool file_is_empty(char *filename) {
  return external_file_is_empty(filename);
}

bool file_is_writable(char *filename) {
  return external_file_is_writable(filename);
}

void file_create(char *filename) {
  external_file_create(filename);
}

void file_remove(char *filename) {
  external_file_remove(filename);
}

bool file_try_remove(char *filename) {
  return external_file_try_remove(filename);
}

void file_remove_partial(char *filename) {
  external_file_remove_partial(filename);
}

void file_remove_both(char *filename) {
  external_file_remove_both(filename);
}

void file_rename(char *orig_filename, char *new_filename) {
  external_file_rename(orig_filename, new_filename);
}

ulong blob_new_file_contents(byte **p_this, File *f) {
  if (file_is_memory(f)) {
    return blob_new_memory_file_contents(p_this, f->_mf);
  }

  return blob_new_external_file_contents(p_this, (ExternalFile *)f->_ef);
}

ulong blob_new_file_contents_from_filename(byte **p_this, char *filename) {
  return blob_new_external_file_contents_from_filename(p_this, filename);
}

void string_new_file_contents(char **p_this, File *f) {
  if (file_is_memory(f)) {
    string_new_memory_file_contents(p_this, f->_mf);
  }

  string_new_external_file_contents(p_this, (ExternalFile *)f->_ef);
}

void string_new_file_contents_from_filename(char **p_this, char *filename) {
  string_new_external_file_contents_from_filename(p_this, filename);
}

sbyte file_progress_status(char *filename) {
  return external_file_progress_status(filename);
}

sbyte files_progress_status(ulong num_filenames, char **filenames) {
  return external_files_progress_status(num_filenames, filenames);
}


void load_label(char *s, File *f) { \
  char *_s = null;
  ulong len;

  load_value(len, f);

  if (len != string_length(s)) {
    throw( \
        "File corrupted: expected label '%s' but loaded a label length of %lu "
        "rather than %lu",
        s,
        len,
        string_length(s));
  }

  string_new_load_from_length(&_s, len, f);

  if (!string_equals(_s, s)) {
    throw("File corrupted: read label '%s' instead of expected '%s'", _s, s);
  }

  string_delete(&_s);
}

static void new_vf_(File **p_this,
    char *mode,
    char *fmt,
    va_list ap1,
    va_list ap2) {
  char *_filename = null;

  string_new_vf(&_filename, fmt, ap1, ap2);
  file_new(p_this, _filename, mode);
  string_delete(&_filename);
}

static bool try_new_vf_(
    File **p_this,
    char *mode,
    char *fmt,
    va_list ap1,
    va_list ap2) {
  bool success;
  char *_filename = null;

  string_new_vf(&_filename, fmt, ap1, ap2);
  success = file_try_new(p_this, _filename, mode);
  string_delete(&_filename);

  return success;
}
