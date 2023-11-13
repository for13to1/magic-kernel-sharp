#include "Swappable.h"
#include <limits.h>

static ulong allocated_ = 0;
static ulong max_ = 0;
static ulong swapped_ = 0;
static ulong in_use_ = 0;
static Lock lock_ = LOCK_INITIALIZER;
static DynamicReferenceSet *_in_use_ = null;
static Lru *_lru_ = null;

static void in_use_print_(SwappableValues *this, void *x);


void swappable_values_newI(
    SwappableValues **p_this,
    ulong num_bytes,
    char *name) {
  SwappableValues *this;

  this = newo(p_this);

  this->numBytes = num_bytes;

  string_new_copy(&this->_name, name);

  while (*this->_name == '(') {
    string_renew_shorten_both_ends(&this->_name, 1);
  }

  while (*this->_name == '&') {
    string_renew_shorten_from_start(&this->_name, 1);
  }

  if (max_ == 0) {
    lock(&lock_);

    if (max_ == 0) {
      max_ = 3 * memory_get_total() / 4;
    }

    unlock(&lock_);
  }

  if (_in_use_ == null) {
    lock(&lock_);

    if (_in_use_ == null) {
      dynamic_reference_set_new_specify(
          &_in_use_,
          null,
          swappable_values_print);
    }

    unlock(&lock_);
  }
}

void swappable_values_newmI(
    SwappableValues ***p_this,
    ulong n0,
    ulong num_bytes,
    char *name) {
  ulong u;
  SwappableValues **this;

  new(p_this, n0);
  this = *p_this;

  for (u = 0; u < n0; u++) {
    swappable_values_newI(&this[u], num_bytes, name);
  }
}

void swappable_values_delete(SwappableValues **p_this) {
  SwappableValues *this;

  if ((this = *p_this) == null) {
    return;
  }

  if (this->refCount > (ulong)LONG_MAX) {
    throw("Swappable somehow has a reference count of %ld",
        (long)this->refCount);
  }

  if (this->refCount > 0) {
    throw("Destructor called on swappable values object but %lu hold%s "
        "still outstanding",
        this->refCount,
        plural(this->refCount, " is", "s are"));
  }

  lock(&lock_);

  if (this->_bytes != null) {
    lru_remove(_lru_, this);

    if (lru_is_empty(_lru_)) {
      lru_delete(&_lru_);
    }
  }

  if (this->_tempFilename != null) {
    file_remove(this->_tempFilename->_filename);
    temp_filename_delete(&this->_tempFilename);
  }

  if (this->_bytes != null) {
    deletev(&this->_bytes);

    if (this->numBytes > allocated_) {
      throw("Impossible for %s: %lu v %lu",
          this->_name,
          this->numBytes,
          allocated_);
    }

    allocated_ -= this->numBytes;
  }

  string_delete(&this->_name);
  deleteo(p_this);

  unlock(&lock_);
}

void swappable_values_print(SwappableValues *this, Printer *p) {
  sprint(p, "SwappableValues");

  rprint(p, "Name: %s", this->_name);
  rprint(p, "Address: %p", this);
  rprint(p, "Number of bytes: %lu", this->numBytes);
  rprint(p, "Reference count: %lu", this->refCount);
  rprint(p,
      "Status: %s",
      swappable_values_is_in_storage(this) ? "in storage"
      : swappable_values_is_in_memory(this) ? "in memory"
      : "unaccessed");
  fprint(p);
}

void swappable_values_access(SwappableValues *this, void *po) {
  char *_s1 = null;
  char *_s2 = null;
  char *_s3 = null;
  SwappableValues *other;

  if (*(void **)po != null) {
    throw("Pointer not empty");
  }

  lock(&lock_);

  if (this->refCount++ == 0) {
    in_use_ += this->numBytes;
    dynamic_reference_set_add(_in_use_, this);

    if (this->_bytes != null) {
      lru_remove(_lru_, this);

      if (lru_is_empty(_lru_)) {
        lru_delete(&_lru_);
      }
    }
  }

  if (this->_bytes != null) {
    *(void **)po = this->_bytes;
    unlock(&lock_);

    return;
  }

  while (allocated_ + this->numBytes > max_) {
    if (_lru_ == null) {
      this->refCount--;
      unlock(&lock_);
      string_new_memory(&_s1, this->numBytes);
      string_new_memory(&_s2, max_ - allocated_);
      string_new_memory(&_s3, max_);

      throw("Cannot allocate swappable %s: only %s of %s not in use",
          _s1,
          _s2,
          _s3);
    }

    other = lru_pop(_lru_);

    if (lru_is_empty(_lru_)) {
      lru_delete(&_lru_);
    }

    temp_filename_new(&other->_tempFilename, "swp");
    external_file_write_bytes_to_filename(
        other->_tempFilename->_filename,
        other->_bytes,
        other->numBytes);
    temp_filename_unlock(other->_tempFilename);
    deletev(&other->_bytes);

    swapped_ += other->numBytes;

    if (other->numBytes > allocated_) {
      throw("Impossible: %lu v %lu", other->numBytes, allocated_);
    }

    allocated_ -= other->numBytes;
  }

  new(&this->_bytes, this->numBytes);
  allocated_ += this->numBytes;

  if (this->_tempFilename != null) {
    external_file_read_bytes_from_filename(
        this->_tempFilename->_filename,
        this->_bytes,
        this->numBytes);
    file_remove(this->_tempFilename->_filename);
    temp_filename_delete(&this->_tempFilename);

    if (this->numBytes > swapped_) {
      throw("Impossible: %lu v %lu", this->numBytes, swapped_);
    }

    swapped_ -= this->numBytes;
  }

  *(void **)po = this->_bytes;

  unlock(&lock_);
}

void swappable_values_accessm(SwappableValues **this, ulong n0, void *ppo) {
  ulong u;
  void **po;

  new((void ***)ppo, n0);
  po = *(void ***)ppo;

  for (u = 0; u < n0; u++) {
    swappable_values_access(this[u], &po[u]);
  }
}

void swappable_values_release(SwappableValues *this, void *po) {
  lock(&lock_);

  if (this->refCount == 0) {
    unlock(&lock_);

    throw("All holds of this swappable values object have already "
        "been released");
  }

  if (--this->refCount == 0) {
    dynamic_reference_set_remove(_in_use_, this);

    if (_lru_ == null) {
      lru_new(&_lru_, swappable_values_print, null);
    }

    lru_add(_lru_, this);

    if (this->numBytes > in_use_) {
      throw("Impossible: %lu v %lu", this->numBytes, in_use_);
    }

    in_use_ -= this->numBytes;
  }

  *(void **)po = null;

  unlock(&lock_);
}

void swappable_values_releasem(SwappableValues **this, ulong n0, void *ppo) {
  ulong u;
  void **po;

  po = *(void ***)ppo;

  for (u = 0; u < n0; u++) {
    swappable_values_release(this[u], &po[u]);
  }

  deletev((void ***)ppo);
}

bool swappable_values_is_unaccessed(SwappableValues *this) {
  return !swappable_values_is_in_memory(this)
    && !swappable_values_is_in_storage(this);
}

bool swappable_values_is_in_memory(SwappableValues *this) {
  return this->_bytes != null;
}

bool swappable_values_is_in_storage(SwappableValues *this) {
  return this->_tempFilename != null;
}

void swappable_newI(
    Swappable **p_this,
    void saver(void **this, struct File *f),
    ulong destroy(void **this),
    ulong loader(void **this, struct File *f)) {
  Swappable *this;

  this = newo(p_this);

  this->saver = saver;
  this->destroy = destroy;
  this->loader = loader;
  lock_initialize(&this->lock);
}

void swappables_newI(
    Swappable ***p_this,
    ulong n,
    void saver(void **this, struct File *f),
    ulong destroy(void **this),
    ulong loader(void **this, struct File *f)) {
  ulong u;
  Swappable **this;

  new(p_this, n);
  this = *p_this;

  for (u = 0; u < n; u++) {
    swappable_newI(&this[u], saver, destroy, loader);
  }
}

void swappable_delete(Swappable **p_this) {
  Swappable *this;

  if ((this = *p_this) == null) {
    return;
  }

  if (this->po != null) {
    if (this->_file == null) {
      throw("Cannot destroy the swappable: "
          "it has %lu unreleased reacquisition%s",
          this->refCount,
          plural_s(this->refCount));
    }

    file_reacquire(this->_file);
    file_delete(&this->_file);
  }

  deleteo(p_this);
}

void swappable_deposit(Swappable *this, void *po) {
  if (*(void **)po == null) {
    throw("Depositing pointer is empty");
  }

  lock(&this->lock);

  if (this->_file != null || this->po != null) {
    unlock(&this->lock);
    throw("An object has already been deposited in this swappable");
  }

  file_new_memory(&this->_file);
  this->saver(*(void **)po, this->_file);
  this->destroy(po);
  file_rewind(this->_file);
  file_release(this->_file);
  this->po = po;

  unlock(&this->lock);
}

void swappables_deposit(Swappable **this, ulong n, void *ppo) {
  ulong u;

  for (u = 0; u < n; u++) {
    swappable_deposit(this[u], &((void **)ppo)[u]);
  }
}

void swappable_withdraw(Swappable *this) {
  lock(&this->lock);

  if (this->_file == null) {
    unlock(&this->lock);

    if (this->po == null) {
      throw("No object has been deposited in this swappable");
    }

    throw("Cannot withdraw object from the swappable: "
        "it has %lu unreleased reacquisition%s",
        this->refCount,
        plural_s(this->refCount));
  }

  if (*this->po != null) {
    unlock(&this->lock);
    throw("Another object has been created in the pointer for this swappable");
  }

  file_reacquire(this->_file);
  this->loader(this->po, this->_file);
  file_delete(&this->_file);
  this->po = null;

  unlock(&this->lock);
}

void swappable_reacquire(Swappable *this) {
  lock(&this->lock);

  if (this->_file == null) {
    if (this->po == null) {
      unlock(&this->lock);
      throw("No object has been deposited in this swappable");
    }

    if (this->refCount == 0) {
      unlock(&this->lock);

      throw("Impossible");
    }

    this->refCount++;

    unlock(&this->lock);

    return;
  }

  if (*this->po != null) {
    unlock(&this->lock);
    throw("Another object has been created in the pointer for this swappable");
  }

  file_reacquire(this->_file);
  this->loader(this->po, this->_file);
  file_delete(&this->_file);
  this->refCount = 1;

  unlock(&this->lock);
}

void swappables_reacquire(Swappable **a, ulong n) {
  ulong u;

  for (u = 0; u < n; u++) {
    swappable_reacquire(a[u]);
  }
}

void swappable_release(Swappable *this) {
  lock(&this->lock);

  if (this->_file != null) {
    unlock(&this->lock);
    throw("There are no unreleased reacquisitions for this swappable");
  }

  if (this->po == null) {
    unlock(&this->lock);
    throw("No object has been deposited in this swappable");
  }

  if (this->refCount == 0) {
    unlock(&this->lock);
    throw("Impossible");
  }

  if (--this->refCount == 0) {
    file_new_memory(&this->_file);
    this->saver(*this->po, this->_file);
    this->destroy(this->po);
    file_rewind(this->_file);
    file_release(this->_file);
  }

  unlock(&this->lock);
}

void swappables_release(Swappable **a, ulong n) {
  ulong u;

  for (u = 0; u < n; u++) {
    swappable_release(a[u]);
  }
}

ulong swappable_set_in_memory_limit(ulong max_bytes) {
  ulong previous;

  if (max_bytes == 0) {
    throw("Swappable in-memory limit cannot be zero bytes");
  }

  previous = max_;
  max_ = max_bytes;

  return previous;
}

void swappable_static_print(void *dummy, Printer *p) {
  char *_s = null;

  sprint(p, "Swappable factory");

  if (max_ == 0) {
    rprint(p, "Not initialized");
    fprint(p);

    return;
  }

  sprint(p, "In memory");
  string_new_memory(&_s, max_);
  rprint(p, "Maximum: %s", _s);
  string_delete(&_s);
  sprint(p, "Allocated");
  string_new_memory(&_s, allocated_);
  rprint(p, "%s", _s);
  string_delete(&_s);

  if (allocated_ > 0) {
    sprint(p, "In use");
    string_new_memory(&_s, in_use_);
    rprint(p, "%s", _s);
    string_delete(&_s);

    if (in_use_ > 0) {
      sprint(p, "Objects");
      dynamic_reference_set_foreach(_in_use_, in_use_print_, p)
        fprint(p);
    }

    fprint(p);
  }

  fprint(p);
  fprint(p);

  if (swapped_ > 0) {
    string_new_memory(&_s, swapped_);
    rprint(p, "Swapped out: %s", _s);
    string_delete(&_s);
  }

  fprint(p);
}

void string_new_swappable_status(char **p_this, bool parenthesize) {
  char *left;
  char *_allocated = null;
  char *_swapped = null;

  if (allocated_ == 0) {
    return;
  }

  left = parenthesize ? "(" : "";
  string_new_memory(&_allocated, allocated_);
  string_new_memory(&_swapped, swapped_);

  string_new_f(p_this, "%s%s used", left, _allocated);

  if (swapped_ > 0) {
    string_renew_append_f(p_this, ", %s swapped", _swapped);
  }

  if (parenthesize) {
    string_renew_append_f(p_this, ")");
  }

  string_delete(&_allocated);
  string_delete(&_swapped);
}

void swappable_progress(
    char **p_this,
    ulong count,
    ulong total_count,
    void *x) {
  string_new_swappable_status(p_this, true);
}


static void in_use_print_(SwappableValues *this, void *x) {
  char *_s = null;
  Printer *p;

  p = x;

  string_new_memory(&_s, this->numBytes);
  rprint(p, "%s: %s", this->_name, _s);
  string_delete(&_s);
}
