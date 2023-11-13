#include "MemoryFile.h"
#include <ctype.h>

static void check_access_(MemoryFile *this);

static void check_lengthen_(MemoryFile *this);


void memory_file_new(MemoryFile **p_this) {
  MemoryFile *this;

  this = newo(p_this);

  new(&this->_bytes, this->numBytes = 1);
}

void memory_file_delete(MemoryFile **p_this) {
  MemoryFile *this;

  if ((this = *p_this) == null) {
    return;
  }

  if (this->_swappable != null) {
    throw("Memory file is released; must be reacquired before being destroyed");
  }

  deletev(&this->_bytes);

  deleteo(p_this);
}

void memory_file_print(MemoryFile *this, Printer *p) {
  bool released;
  byte hexits;
  byte block;
  byte b;
  byte d;
  char *_s = null;
  ulong size;
  ulong position;
  ulong u;

  sprint(p, "MemoryFile");
  rprint(p, "Released: %s", bool_string(released = this->_swappable != null));
  size = memory_file_get_size(this);
  position = memory_file_get_position(this);
  rprint(p, "Size: %lu", size);
  rprint(p, "Current position: %lu", position);

  if (!released && size > 0) {
    sprint(p, "Contents");
    memory_file_rewind(this);
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
                memory_file_read_byte(this));
          }
        }

        string_renew_append(&_s, " ");
      }

      string_renew_append(&_s, " ");
      u -= 16;
      memory_file_set_position(this, u);

      for (b = 0; b < 16; b++, u++) {
        if (u >= size) {
          d = ' ';
        } else {
          d = (char)memory_file_read_byte(this);

          if (!isprint(d)) {
            d = '.';
          }
        }

        string_renew_append_char(&_s, d);
      }

      rprint(p, "%s", _s);
      string_delete(&_s);
    }

    memory_file_set_position(this, position);
    fprint(p);
  }

  fprint(p);
}

void memory_file_release(MemoryFile *this) {
  byte *_bytes = null;

  if (this->_swappable != null) {
    throw("Memory file is already released");
  }

  swappable_bytes_new(&this->_swappable, this->size);
  swappable_values_access(this->_swappable, &_bytes);
  copy_memory(this->_bytes, _bytes, this->size);
  deletev(&this->_bytes);
  swappable_values_release(this->_swappable, &_bytes);
}

void memory_file_reacquire(MemoryFile *this) {
  byte *_bytes = null;

  if (this->_swappable == null) {
    throw("Memory file is not released");
  }

  new(&this->_bytes, this->size);
  swappable_values_access(this->_swappable, &_bytes);
  copy_memory(_bytes, this->_bytes, this->size);
  swappable_values_release(this->_swappable, &_bytes);
  swappable_values_delete(&this->_swappable);
}

ulong memory_file_get_size(MemoryFile *this) {
  return this->size;
}

ulong memory_file_get_position(MemoryFile *this) {
  return this->position;
}

void memory_file_set_position(MemoryFile *this, ulong position) {
  check_access_(this);

  this->position = position;
  check_lengthen_(this);
}

bool memory_file_is_at_end(MemoryFile *this) {
  return memory_file_get_position(this) == memory_file_get_size(this);
}

void memory_file_rewind(MemoryFile *this) {
  check_access_(this);

  memory_file_set_position(this, 0);
}

void memory_file_fast_forward(MemoryFile *this) {
  check_access_(this);

  memory_file_set_position(this, memory_file_get_size(this));
}

void memory_file_write_byte(MemoryFile *this, byte b) {
  check_access_(this);

  this->_bytes[this->position++] = b;
  check_lengthen_(this);
}

void memory_file_write_bytes(MemoryFile *this, byte *bs, ulong num_bytes) {
  ulong current_position;

  current_position = this->position;
  this->position += num_bytes;
  check_lengthen_(this);
  copy_memory(bs, this->_bytes + current_position, num_bytes);
}

void memory_file_write_bytesm(
    MemoryFile *this,
    byte **bm,
    ulong n0,
    ulong num_bytes) {
  ulong current_position;

  current_position = this->position;
  this->position += n0 * num_bytes;
  check_lengthen_(this);
  copy_memorym_flatten(bm, this->_bytes + current_position, n0, num_bytes);
}

byte memory_file_read_byte(MemoryFile *this) {
  byte result = 0;

  check_access_(this);

  if (!memory_file_try_read_byte(this, &result)) {
    throw("Cannot read byte: end of file");
  }

  return result;
}

bool memory_file_try_read_byte(MemoryFile *this, byte *b) {
  check_access_(this);

  if (this->position == this->size) {
    return false;
  }

  *b = this->_bytes[this->position++];
  check_lengthen_(this);

  return true;
}

void memory_file_read_bytes(MemoryFile *this, byte *bs, ulong num_bytes) {
  copy_memory(this->_bytes + this->position, bs, num_bytes);
  this->position += num_bytes;
}

void memory_file_read_bytesm(
    MemoryFile *this,
    byte **bm,
    ulong n0,
    ulong num_bytes) {
  copy_memorym_unflatten(this->_bytes + this->position, bm, n0, num_bytes);
  this->position += n0 * num_bytes;
}

bool memory_file_contents_equals_blob(
    MemoryFile *this,
    ulong len,
    byte *blob) {
  byte read;
  byte *p;
  byte *e;

  check_access_(this);
  memory_file_rewind(this);

  for (p = blob, e = p + len; p != e; p++) {
    if (!memory_file_try_read_byte(this, &read)) {
      return false;
    }

    if (read != *p) {
      return false;
    }
  }

  if (!memory_file_is_at_end(this)) {
    return false;
  }

  return true;
}

void memory_file_assert_contents_equals_blob(
    MemoryFile *this,
    ulong len,
    byte *blob) {
  byte read = 0;
  byte *p;
  byte *e;

  check_access_(this);
  memory_file_rewind(this);

  for (p = blob, e = p + len; p != e; p++) {
    if (!memory_file_try_read_byte(this, &read)) {
      throw("Memory file shorter than expected: "
          "actual %lu, expected %lu",
          p - blob,
          len);
    }

    if (read != *p) {
      throw("Disagreement in memory file at offset %lu: "
          "actual 0x%02x, expected 0x%02x",
          p - blob,
          read,
          *p);
    }
  }

  if (!memory_file_is_at_end(this)) {
      throw("Memory file longer than expected: "
          "actual %lu, expected %lu",
          memory_file_get_size(this),
          len);
  }
}

ulong blob_new_memory_file_contents(byte **p_this, MemoryFile *mf) {
  byte *e;
  byte *p;
  ulong len;
  byte *this;

  len = memory_file_get_size(mf);

  new(p_this, len);
  this = *p_this;

  memory_file_rewind(mf);

  for (p = this, e = p + len; p != e; p++) {
    *p = memory_file_read_byte(mf);
  }

  return len;
}

void string_new_memory_file_contents(char **p_this, MemoryFile *mf) {
  char c;
  ulong n;
  ulong u;
  char *this;

  check_access_(mf);
  n = memory_file_get_size(mf);
  new(p_this, n + 1);
  this = *p_this;

  memory_file_rewind(mf);

  for (u = 0; u < n; u++) {
    this[u] = c = (char)memory_file_read_byte(mf);

    if (c == '\0') {
      throw("Invalid '\\0' found in memory file being read into a string");
    }
  }

  this[u] = '\0';
}


static void check_access_(MemoryFile *this) {
  if (this->_swappable != null) {
    throw("No actions can be taken on a released memory file");
  }
}

static void check_lengthen_(MemoryFile *this) {
  ulong orig_num_bytes;

  if (this->position > this->size) {
    this->size = this->position;
  }

  if (this->size < this->numBytes) {
    return;
  }

  orig_num_bytes = this->numBytes;

  while (this->numBytes <= this->size) {
    this->numBytes *= 2;
  }

  renewup(&this->_bytes, orig_num_bytes, this->numBytes);
}
