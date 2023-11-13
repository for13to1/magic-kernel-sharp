#include "ExternalFile.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

typedef struct {
  FILE **pf;
  char *filename;
  char *new_filename;
  char *mode;
} Info_;

static bool try_open_(FILE **pf, char *filename, char *mode);

static void parse_mode_(ExternalFile *this, char *mode);

static char *make_partial_filename_(char **p_this, char *filename);


void external_file_new(ExternalFile **p_this, char *filename, char *mode) {
  if (!external_file_try_new(p_this, filename, mode)) {
    /* If we ever really need to deal with too many open files, it's EMFILE.
     */
    throw("Could not open external file '%s' in mode '%s': Error %d: %s",
        filename,
        mode,
        errno,
        strerror(errno));
  }
}

bool external_file_try_new(ExternalFile **p_this, char *filename, char *mode) {
  char *_other_filename = null;
  char *use_filename;
  ExternalFile *this;

  if (mode == null) {
    throw("Mode cannot be null");
  }

  if (filename == null) {
    if (string_equals(mode, "stdin")) {
      external_file_new_wrap_ansifile(p_this, stdin, "rt");
    } else if (string_equals(mode, "stdout")) {
      external_file_new_wrap_ansifile(p_this, stdout, "wt");
    } else if (string_equals(mode, "stderr")) {
      external_file_new_wrap_ansifile(p_this, stderr, "wt");
    } else {
      throw("Mode for null filename must be 'stdin', 'stdout' or 'stderr'");
    }

    this = *p_this;

    string_delete(&this->_filename);
    string_new_concat_until_null(&this->_filename, "(", mode, ")", null);

    return true;
  } else if (string_equals(filename, "-")) {
    external_file_new_wrap_ansifile(p_this, stdout, mode);
    this = *p_this;
    string_delete(&this->_mode);

    if (this->read) {
      this->ansifile = stdin;
      string_renew_copy(&this->_filename, "(stdin)");
      parse_mode_(this, "rt");
    } else {
      string_renew_copy(&this->_filename, "(stdin)");
      parse_mode_(this, "wt");
    }

    return true;
  }

  this = newo(p_this);

  string_new_copy(&this->_filename, filename);

  parse_mode_(this, mode);

  if (this->write) {
    if (string_equals(filename, "/dev/null")) {
      this->devnull = true;

      return true;
    }

    if (external_file_exists(filename)) {
      if (!external_file_is_writable(filename)
          || string_equals(filename, "/dev/null")) {
        external_file_delete(p_this);

        return false;
      }

      external_file_remove(filename);
    }

    if (string_ends_with(filename, ".gz", true)) {
      string_new_shorten(&_other_filename, filename, 3);
    } else {
      string_new_f(&_other_filename, "%s.gz", filename);
    }

    if (external_file_exists(_other_filename)) {
      if (!external_file_is_writable(_other_filename)) {
        string_delete(&_other_filename);
        external_file_delete(p_this);

        return false;
      }

      external_file_remove(_other_filename);
    }

    string_delete(&_other_filename);
    use_filename = make_partial_filename_(&this->_partialFilename, filename);
  } else {
    use_filename = filename;
  }

  if (string_ends_with(filename, ".gz", true)) {
    if ((this->gzfile = gzopen(use_filename, this->_mode)) == null) {
      external_file_delete(p_this);

      return false;
    }
  } else {
    if (!try_open_(&this->ansifile, use_filename, this->_mode)) {
      external_file_delete(p_this);

      return false;
    }
  }

  return true;
}

void external_file_new_wrap_ansifile(
    ExternalFile **p_this,
    FILE *ansifile,
    char *mode) {
  ExternalFile *this;

  this = newo(p_this);

  this->wrap = true;
  parse_mode_(this, mode);
  this->ansifile = ansifile;
  string_new_copy(&this->_filename, "(wrapped ANSI FILE)");
}

void external_file_delete(ExternalFile **p_this) {
  bool success;
  ExternalFile *this;

  if ((this = *p_this) == null) {
    return;
  }

  if (!this->devnull) {
    success = (this->ansifile != null || this->gzfile != null);

    if (!this->wrap && this->ansifile != null) {
      fclose(this->ansifile);
    }

    if (this->gzfile != null) {
      gzclose(this->gzfile);
    }

    if (success && this->_partialFilename != null) {
      external_file_rename_if_exists(this->_partialFilename, this->_filename);
    }
  }

  string_delete(&this->_mode);
  string_delete(&this->_partialFilename);
  string_delete(&this->_filename);

  deleteo(p_this);
}

void external_file_print(ExternalFile *this, Printer *p) {
  byte hexits;
  byte block;
  byte b;
  byte d;
  char *_s = null;
  ulong size;
  ulong position;
  ulong u;

  sprint(p, "ExternalFile");
  rprint(p, "Filename: '%s'", this->_filename);

  if (this->_partialFilename != null) {
    rprint(p, "Partial filename: '%s'", this->_partialFilename);
  }

  rprint(p, "Gzipped: %s", bool_string(external_file_is_gzipped(this)));
  rprint(p, "Wrapped: %s", bool_string(this->wrap));
  rprint(p, "Mode: '%s'", this->_mode);

  if (!this->wrap) {
    size = external_file_get_size(this);
    position = external_file_get_position(this);
    rprint(p, "Size: %lu", size);
    rprint(p, "Current position: %lu", position);
  }

  rprint(p, "Read: %s", bool_string(this->read));
  rprint(p, "Write: %s", bool_string(this->write));
  rprint(p, "Append: %s", bool_string(this->append));
  rprint(p, "Update: %s", bool_string(this->update));

  if (!this->wrap && (this->read || this->update)) {
    sprint(p, "Contents");
    external_file_rewind(this);
    hexits = ulong_hexit_width(size - 1);

    for (u = 0; u < size;) {
      string_new_f(&_s, "%0*lx: ", (int)hexits, u);

      for (block = 0; block < 8; block++) {
        for (b = 0; b < 2; b++, u++) {
          if (u >= size) {
            string_renew_append(&_s, "  ");
          } else {
            string_renew_append_f(
                &_s,
                "%02x",
                external_file_read_byte(this));
          }
        }

        string_renew_append(&_s, " ");
      }

      string_renew_append(&_s, " ");
      u -= 16;
      external_file_set_position(this, u);

      for (b = 0; b < 16; b++, u++) {
        if (u >= size) {
          d = ' ';
        } else {
          d = (char)external_file_read_byte(this);

          if (!isprint(d)) {
            d = '.';
          }
        }

        string_renew_append_char(&_s, d);
      }

      rprint(p, "%s", _s);
      string_delete(&_s);
    }

    external_file_set_position(this, position);

    fprint(p);
  }

  fprint(p);
}

bool external_file_is_gzipped(ExternalFile *this) {
  return this->gzfile != null;
}

void external_file_write_byte(ExternalFile *this, byte b) {
  if (this->devnull) {
    return;
  }

  if (external_file_is_gzipped(this)) {
    if (gzputc(this->gzfile, b) < 0) {
      throw("Could not write byte to '%s'", this->_filename);
    }
  } else {
    if (fputc(b, this->ansifile) == EOF) {
      throw("Could not write byte to '%s'", this->_filename);
    }
  }
}

void external_file_write_bytes(ExternalFile *this, byte *bs, ulong n) {
  if (this->devnull) {
    return;
  }

  if (external_file_is_gzipped(this)) {
    if (gzwrite(this->gzfile, bs, n) != n) {

      throw("Could not write %lu byte%s to '%s'",
          n,
          plural_s(n),
          this->_filename);
    }
  } else {
    if (fwrite(bs, 1, n, this->ansifile) != n) {
      throw("Could not write %lu byte%s to '%s'",
          n,
          plural_s(n),
          this->_filename);
    }
  }
}

void external_file_write_bytes_to_filename(char *filename, byte *bs, ulong n) {
  ExternalFile *_ef = null;

  external_file_new(&_ef, filename, "w");
  external_file_write_bytes(_ef, bs, n);
  external_file_delete(&_ef);
}

byte external_file_read_byte(ExternalFile *this) {
  byte result = 0;

  if (!external_file_try_read_byte(this, &result)) {
    throw("Could not read byte from '%s', position %lu",
        this->_filename,
        external_file_get_position(this));
  }

  return result;
}

bool external_file_try_read_byte(ExternalFile *this, byte *b) {
  int c;

  if (external_file_is_gzipped(this)) {
    if ((c = gzgetc(this->gzfile)) < 0) {
      return false;
    }
  } else {
    if ((c = fgetc(this->ansifile)) == EOF) {
      return false;
    }
  }

  *b = (byte)c;
  return true;
}

void external_file_read_bytes(ExternalFile *this, byte *bs, ulong n) {
  if (bs == null) {
    external_file_set_position(this, external_file_get_position(this) + n);
  } else if (external_file_is_gzipped(this)) {
    if (gzread(this->gzfile, bs, n) != n) {
      throw("Could not read %lu byte%s from '%s'",
          n,
          plural_s(n),
          this->_filename);
    }
  } else {
    if (fread(bs, 1, n, this->ansifile) != n) {
      throw("Could not read %lu byte%s from '%s'",
          n,
          plural_s(n),
          this->_filename);
    }
  }
}

void external_file_read_bytes_from_filename(char *filename, byte *bs, ulong n) {
  ExternalFile *_ef = null;

  external_file_new(&_ef, filename, "r");
  external_file_read_bytes(_ef, bs, n);
  external_file_delete(&_ef);
}

ulong external_file_get_size(ExternalFile *this) {
  ulong size;
  fpos_t pos;

  if (external_file_is_gzipped(this)) {
    throw("Do not try to get the size of gzipped file '%s'", this->_filename);
  }

  if (fgetpos(this->ansifile, &pos) != 0) {
    throw("Could not save file position for '%s': %s",
        this->_filename,
        strerror(errno));
  }

  if (fseek(this->ansifile, 0, SEEK_END) != 0) {
    throw("Could not set file '%s' position to end of file: %s",
        this->_filename,
        strerror(errno));
  }

  size = external_file_get_position(this);

  if (fsetpos(this->ansifile, &pos) != 0) {
    throw("Could not restore file position for '%s': %s",
        this->_filename,
        strerror(errno));
  }

  return size;
}

ulong external_file_get_position(ExternalFile *this) {
  long position;

  if (external_file_is_gzipped(this)) {
    throw("Do not try to get the position of gzipped file '%s'",
        this->_filename);
  }

  if (this->text) {
    throw("Do not try to get position for %s in text mode '%s'",
        this->_filename,
        this->_mode);
  }

  if ((position = ftell(this->ansifile)) < 0) {
    throw("Could not get position of file '%s'", this->_filename);
  }

  return (ulong)position;
}

void external_file_set_position(ExternalFile *this, ulong position) {
  if (external_file_is_gzipped(this)) {
    throw("Do not try to set the position of gzipped file '%s'",
        this->_filename);
  }

  if (this->text) {
    throw("Do not try to set position for %s in text mode '%s'",
        this->_filename,
        this->_mode);
  }

  if (fseek(this->ansifile, position, SEEK_SET) != 0) {
    throw("Could not set file '%s' position to %lu in mode '%s': %s",
        this->_filename,
        position,
        this->_mode,
        strerror(errno));
  }
}

void external_file_rewind(ExternalFile *this) {
  if (external_file_is_gzipped(this)) {
    gzrewind(this->gzfile);
  } else {
    external_file_set_position(this, 0);
  }
}

void external_file_fast_forward(ExternalFile *this) {
  if (fseek(this->ansifile, 0, SEEK_END) != 0) {
    throw("Could not fast-forward file '%s' in mode '%s': %s",
        this->_filename,
        this->_mode,
        strerror(errno));
  }
}

void external_file_flush(ExternalFile *this) {
  if (external_file_is_gzipped(this)) {
    gzflush(this->gzfile, Z_FINISH);
  } else {
    fflush(this->ansifile);
  }
}

bool external_file_contents_equals_blob(
    ExternalFile *this,
    ulong len,
    byte *blob) {
  byte read;
  byte *p;
  byte *e;

  external_file_rewind(this);

  for (p = blob, e = p + len; p != e; p++) {
    if (!external_file_try_read_byte(this, &read)) {
      return false;
    }

    if (read != *p) {
      return false;
    }
  }

  if (external_file_try_read_byte(this, &read)) {
    return false;
  }

  return true;
}

void external_file_assert_contents_equals_blob(
    ExternalFile *this,
    ulong len,
    byte *blob) {
  byte read = 0;
  byte *p;
  byte *e;

  external_file_rewind(this);

  for (p = blob, e = p + len; p != e; p++) {
    if (!external_file_try_read_byte(this, &read)) {
      throw("File '%s' had length %lu; expected %lu",
          this->_filename,
          p - blob,
          len);
    }

    if (read != *p) {
      throw("File '%s' disagreement at offset %lu: "
          "expected 0x%02x, actual 0x%02x",
          this->_filename,
          p - blob,
          *p,
          read);
    }
  }

  if (external_file_try_read_byte(this, &read)) {
    throw("File '%s' is longer than expected: actual %lu, expected %lu",
        this->_filename,
        external_file_get_size(this),
        len);
  }
}

bool external_file_exists(char *filename) {
  FILE *f;

  if (try_open_(&f, filename, "r")) {
    fclose(f);

    return true;
  }

  return false;
}

bool external_file_exists_wait(char *filename, byte max_seconds) {
  uint seconds;

  for (seconds = 0; seconds < max_seconds; seconds++) {
    if (external_file_exists(filename)) {
      return true;
    }

    sleep(1);
  }

  return false;
}

bool external_file_partial_exists(char *filename) {
  bool result;
  char *_partial_filename = null;

  make_partial_filename_(&_partial_filename, filename);
  result = external_file_exists(_partial_filename);
  string_delete(&_partial_filename);

  return result;
}

bool external_file_either_exists(char *filename) {
  return
    external_file_exists(filename) ||
    external_file_partial_exists(filename);
}

bool external_file_is_empty(char *filename) {
  bool empty;
  byte b;
  FILE *f;

  if (!try_open_(&f, filename, "r")) {
    throw("Could not open '%s' for reading to check if it is empty", filename);
  }

  empty = fread(&b, 1, 1, f) != 1;
  fclose(f);

  return empty;
}

bool external_file_is_writable(char *filename) {
  FILE *f;

  if (try_open_(&f, filename, "r+")) {
    fclose(f);

    return true;
  }

  if (!try_open_(&f, filename, "w")) {
    return false;
  }

  fclose(f);
  external_file_remove(filename);

  return true;
}

void external_file_create(char *filename) {
  FILE *f;

  if (external_file_exists(filename)) {
    throw("File '%s' already exists", filename);
  }

  if (!try_open_(&f, filename, "w")) {
    throw("Could not create file '%s'", filename);
  }

  fclose(f);
}

void external_file_remove(char *filename) {
  if (!external_file_try_remove(filename)) {
    throw("Could not remove file '%s': %s", filename, strerror(errno));
  }
}

void external_file_remove_if_exists(char *filename) {
  external_file_try_remove(filename);
}

bool external_file_try_remove(char *filename) {
  return remove(filename) == 0;
}

void external_file_remove_partial(char *filename) {
  char *_partial_filename = null;

  make_partial_filename_(&_partial_filename, filename);
  external_file_remove(_partial_filename);
  string_delete(&_partial_filename);
}

void external_file_remove_both(char *filename) {
  char *_partial_filename = null;

  if (external_file_exists(filename)) {
    external_file_remove(filename);
  }

  make_partial_filename_(&_partial_filename, filename);

  if (external_file_exists(_partial_filename)) {
    external_file_remove(_partial_filename);
  }

  string_delete(&_partial_filename);
}

void external_file_rename(char *orig_filename, char *new_filename) {
  if (rename(orig_filename, new_filename) != 0) {
    throw("Could not rename file '%s' to '%s': %s",
        orig_filename,
        new_filename,
        strerror(errno));
  }
}

void external_file_rename_if_exists(char *orig_filename, char *new_filename) {
  if (external_file_exists(orig_filename)) {
    external_file_rename(orig_filename, new_filename);
  }
}

ulong blob_new_external_file_contents(byte **p_this, ExternalFile *ef) {
  ulong len;
  File *_f = null;

  file_new_memory_copy_external(&_f, (struct ExternalFile *)ef);
  len = blob_new_file_contents(p_this, _f);
  file_delete(&_f);

  return len;
}

ulong blob_new_external_file_contents_from_filename(
    byte **p_this,
    char *filename) {
  ulong len;
  File *_f = null;

  file_new_memory_copy_from_filename(&_f, filename);
  len = blob_new_file_contents(p_this, _f);
  file_delete(&_f);

  return len;
}

void string_new_external_file_contents(char **p_this, ExternalFile *ef) {
  File *_f = null;

  file_new_memory_copy_external(&_f, (struct ExternalFile *)ef);
  string_new_file_contents(p_this, _f);
  file_delete(&_f);
}

void string_new_external_file_contents_from_filename(
    char **p_this,
    char *filename) {
  File *_f = null;

  file_new_memory_copy_from_filename(&_f, filename);
  string_new_memory_file_contents(p_this, _f->_mf);
  file_delete(&_f);
}

sbyte external_file_progress_status(char *filename) {
  bool exists;
  bool partial_exists;

  partial_exists = external_file_partial_exists(filename);
  exists = external_file_exists(filename);

  if (!partial_exists && !exists) {
    return -1;
  }

  if (partial_exists) {
    return 0;
  }

  return +1;
}

sbyte external_files_progress_status(ulong num_filenames, char **filenames) {
  bool all_not_started;
  bool all_finished;
  int status;
  ulong u;

  all_not_started = true;
  all_finished = true;

  for (u = 0; u < num_filenames; u++) {
    status = external_file_progress_status(filenames[u]);

    if (status <= 0) {
      all_finished = false;
    }

    if (status >= 0) {
      all_not_started = false;
    }
  }

  return all_finished ? +1 : all_not_started ? -1 : 0;
}

FILE *external_file_get_ansifile(ExternalFile *this) {
  if (external_file_is_gzipped(this)) {
    throw("Cannot get ANSI FILE from a gzipped ExternalFile");
  }

  return this->ansifile;
}

bool ansifile_contents_equals_blob(FILE *file, ulong len, byte *blob) {
  byte *p;
  byte *e;

  rewind(file);

  for (p = blob, e = p + len; p < e; p++) {
    if (fgetc(file) != *p) {
      return false;
    }
  }

  if (fgetc(file) != EOF) {
    return false;
  }

  return true;
}

bool ansifile_from_filename_contents_equals_blob(
    char *filename,
    ulong len,
    byte *blob) {
  bool result;
  FILE *file;

  if ((file = fopen(filename, "r")) == null) {
    throw("Could not open file '%s'", filename);
  }

  result = ansifile_contents_equals_blob(file, len, blob);

  fclose(file);

  return result;
}

bool ansifile_contents_equals_string(FILE *file, char *s) {
  return ansifile_contents_equals_blob(file, string_length(s), (byte *)s);
}

bool ansifile_from_filename_contents_equals_string(char *filename, char *s) {
  return ansifile_from_filename_contents_equals_blob(
      filename,
      string_length(s),
      (byte *)s);
}

void external_file_reverse_endianness(byte *in, byte *out, ulong n) {
  byte u;
  byte v;

  for (u = 0, v = n - 1; u < n; u++, v--) {
    out[u] = in[v];
  }
}


void external_file_save_big_endian(ExternalFile *this, byte *bs, ulong n) {
  byte out[8];

  if (n != 8 && n != 4 && n != 2 && n != 1) {
    throw("Impossible: number of bytes (%lu) must be 1, 2, 4, or 8", n);
  }

  external_file_reverse_endianness(bs, out, n);
  external_file_write_bytes(this, out, n);
}

void external_file_load_big_endian(ExternalFile *this, byte *bs, ulong n) {
  byte in[8];

  if (n != 8 && n != 4 && n != 2 && n != 1) {
    throw("Impossible: number of bytes (%lu) must be 1, 2, 4, or 8", n);
  }

  external_file_read_bytes(this, in, n);
  external_file_reverse_endianness(in, bs, n);
}

static void parse_mode_(ExternalFile *this, char *mode) {
  bool includes_b;
  char c;
  char *p;

  includes_b = false;

  for (p = mode; (c = *p) != '\0'; p++) {
    switch (c) {
      case 'r':
        this->read = true;
        break;
      case 'w':
        this->write = true;
        break;
      case 'a':
        this->append = true;
        break;
      case '+':
        this->update = true;
        break;
      case 'b':
        includes_b = true;
        break;
      case 't':
        if (!this->wrap) {
          throw("Text mode not supported for non-wrapped ExternalFile "
              "(mode '%s' file '%s')",
              mode,
              this->_filename);
        }
        this->text = true;
        break;
      default:
        throw("Unknown mode character '%c' in mode string '%s' opening '%s'",
            c,
            mode,
            this->_filename);
        break;
    }
  }

  if (this->append) {
    if (this->read) {
      throw("Cannot include both 'a' and 'r' in mode (mode '%s' file '%s')",
          mode,
          this->_filename);
    }

    if (this->write) {
      throw("Cannot include both 'a' and 'w' in mode (mode '%s' file '%s')",
          mode,
          this->wrap ? "wrapped ANSI FILE" : this->_filename);
    }
  }

  /* Even though there is no text translation mode needed in Unix and Linux,
   * opening in text mode prevents seeking (because on other architectures
   * there is no way of predicting binary offset from text offset). So always
   * open files in ANSI binary mode.
   */
  if (this->wrap || includes_b) {
    string_new_copy(&this->_mode, mode);
  } else {
    string_new_concat_until_null(&this->_mode, mode, "b", null);
  }
}

static char *make_partial_filename_(char **p_this, char *filename) {
  string_new_concat(p_this, filename, ".partial");

  return *p_this;
}


static bool try_open_(FILE **pf, char *filename, char *mode) {
  return (*pf = fopen(filename, mode)) != null;
}
