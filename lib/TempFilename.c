#include "TempFilename.h"

#define ROOT_DIR_ "/tmp/jpc"
#define MAX_FILES_PER_DIR_ 1000
#define DIRNUM_DIGITS_ 3
#define MAX_TRIES_ 1000

static ulong dirnum1_ = 0;
static ulong dirnum2_ = 0;
static ulong serial_ = 0;
static Lock lock_ = LOCK_INITIALIZER;
static Random *_random_ = null;


void temp_filename_new(TempFilename **p_this, char *extension) {
  char *lock_contents = "This is a lock file for a temp filename";
  char *_datetime = null;
  char *_dirname = null;
  char *_filename = null;
  char *_lock_filename = null;
  ushort i;
  ulong random_u;
  ExternalFile *_lock_file = null;
  TempFilename *this;

  if (extension != null && string_contains_char(extension, '/')) {
    throw("Extension '%s' contains '/', which is not allowed", extension);
  }

  this = newo(p_this);

  lock(&lock_);

  string_new_f(
      &_dirname,
      "%s/%0*lu/%0*lu",
      ROOT_DIR_,
      DIRNUM_DIGITS_,
      dirnum1_,
      DIRNUM_DIGITS_,
      dirnum2_);

  if (serial_++ % MAX_FILES_PER_DIR_ == 0) {
    directory_ensure_exists(_dirname);
  }

  if (serial_ % MAX_FILES_PER_DIR_ == 0) {
    if (++dirnum2_ == MAX_FILES_PER_DIR_) {
      dirnum1_++;
      dirnum2_ = 0;
    }
  }

  if (_random_ == null) {
    random_new(&_random_);
  }

  for (i = 0; i < MAX_TRIES_; i++) {
    random_u = random_modulo(_random_, 1000000);

    string_new_datetime_current(
        &_datetime,
        false,
        "",
        "",
        "",
        "",
        true,
        true,
        true,
        true,
        6);
    string_new_f(
        &_filename,
        "%s/tmp-%s-%06lu%s%s",
        _dirname,
        _datetime,
        random_u,
        extension == null ? "" : ".",
    extension == null ? "" : extension);
    string_delete(&_datetime);
    string_new_concat(&_lock_filename, _filename, ".lock");

    if (!external_file_exists(_lock_filename)
        && external_file_try_new(&_lock_file, _lock_filename, "w")) {
      external_file_write_bytes(
          _lock_file,
          (byte *)lock_contents,
          string_length(lock_contents) + 1);

      break;
    }

    string_delete(&_lock_filename);
    string_delete(&_filename);
  }

  if (i == MAX_TRIES_) {
    throw("Could not create a temp filename, after even %u tries",
        MAX_TRIES_);
  }

  donate(&_filename, &this->_filename);
  donate(&_lock_filename, &this->_lockFilename);
  donate(&_lock_file, &this->_lockFile);

  string_delete(&_dirname);

  unlock(&lock_);
}

void temp_filename_new_keep(TempFilename **p_this, char *extension, bool keep) {
  temp_filename_new(p_this, extension);

  (*p_this)->keep = keep;
}

void temp_filename_delete(TempFilename **p_this) {
  TempFilename *this;

  if ((this = *p_this) == null) {
    return;
  }

  temp_filename_unlock(this);

  if (!this->keep && external_file_exists(this->_filename)) {
    external_file_try_remove(this->_filename);
  }

  string_delete(&this->_filename);

  deleteo(p_this);
}

void temp_filename_unlock(TempFilename *this) {
  if (this->_lockFile == null) {
    return;
  }

  external_file_delete((ExternalFile **)&this->_lockFile);
  external_file_remove_if_exists(this->_lockFilename);
  string_delete(&this->_lockFilename);
}
