/* See ../_README and jpc.h for a description of jpC.
 */
#define JPC_C
#include "jpc.h"
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

#define MIN_ 100000

#define throw_ \
  throw_lockI(); \
  g_JPC_internal_throw_ = true, \
  g_JPC_file_2_ = g_JPC_file_1_, \
  g_JPC_line_2_ = g_JPC_line_1_, \
  g_JPC_file_1_ = file, \
  g_JPC_line_1_ = line, \
  throwI

typedef struct {
  char *file;
  ulong bytes;
  ulong line;
  void **this;
} NewMPass_;

typedef struct {
  char *file;
  ulong size;
  ulong line;
  ulong *n1s;
  void **this;
} NewRMPass_;

typedef struct {
  char *file;
  ulong line;
  ulong n1;
  ulong size;
  ulong (*destroy)(void **p_this);
  void **this;
} DeleteMPass_;

typedef struct {
  char *file;
  ulong line;
  ulong *n1s;
  ulong size;
  ulong (*destroy)(void **p_this);
  void **this;
} DeleteRMPass_;

typedef struct {
  char *file;
  ulong bytes;
  ulong line;
  void **this;
} DeleteMVPass_;

typedef struct {
  char *file;
  ulong size;
  ulong line;
  ulong *n1s;
  void **this;
} DeleteRMVPass_;

typedef struct {
  char *file;
  ulong bytes;
  ulong line;
  ulong n1;
  void ***this;
} NewT3Pass_;

typedef struct {
  char *file;
  ulong line;
  ulong n1;
  ulong n2;
  ulong bytes;
  ulong (*destroy)(void **p_this);
  void ***this;
} DeleteT3Pass_;

typedef struct {
  char *file;
  ulong bytes;
  ulong line;
  ulong n1;
  void ***this;
} DeleteT3VPass_;

typedef struct {
  char *file;
  ulong bytes;
  ulong line;
  ulong n1;
  ulong n2;
  void ****this;
} NewT4Pass_;

typedef struct {
  char *file;
  ulong line;
  ulong n1;
  ulong n2;
  ulong n3;
  ulong bytes;
  ulong (*destroy)(void **p_this);
  void ****this;
} DeleteT4Pass_;

typedef struct {
  char *file;
  ulong bytes;
  ulong line;
  ulong n1;
  ulong n2;
  void ****this;
} DeleteT4VPass_;

typedef struct {
  byte *src;
  byte *dst;
} CopyPass_;

typedef struct {
  byte **src;
  byte **dst;
  ulong numBytes;
} CopyMPass_;

typedef struct {
  bool *equals;
  byte *a;
  byte *b;
} EqualsPass_;

typedef struct {
  bool *equals;
  byte **a;
  byte **b;
  ulong numBytes;
} EqualsMPass_;

bool g_JPC_internal_throw_ = false;
bool g_JPC_exception_separators_ = true;
byte g_JPC_try_ = 0;
char *exception = null;
char *g_JPC_file_1_ = null;
char *g_JPC_file_2_ = null;
char *g_JPC_orig_exception_ = null;
int g_JPC_line_1_ = -1;
int g_JPC_line_2_ = -1;
jmp_buf g_JPC_jmp_buf_;

static bool allow_exit_normally_ = true;
static Lock throw_lock_ = LOCK_INITIALIZER;

static void newm_(Parallel *par);

static void newrm_(Parallel *par);

static void deletem_(Parallel *par);

static void deleterm_(Parallel *par);

static void deletemv_(Parallel *par);

static void deletermv_(Parallel *par);

static void newt3_(Parallel *par);

static void deletet3_(Parallel *par);

static void deletet3v_(Parallel *par);

static void newt4_(Parallel *par);

static void deletet4_(Parallel *par);

static void deletet4v_(Parallel *par);

static void copy_(Parallel *par);

static void copym_(Parallel *par);

static void copym_flatten_(Parallel *par);

static void copym_unflatten_(Parallel *par);

static void clear_(Parallel *par);

static void equals_(Parallel *par);

static void equalsm_(Parallel *par);


void *newoI(void **p_this, ulong size, char *file, int line) {
  void *this;

  if (p_this == null) {
    throw_("Pointer to pointer null");
  }

  if (*p_this != null) {
    throw_("Pointer not empty");
  }

  if (size == 0) {
    size = 1;
  }

  if ((this = *p_this = calloc(size, 1)) == null) {
    throw_("Out of memory allocating %lu bytes", size);
  }

  return this;
}

void *newI(void **p_this, ulong n, ulong size, char *file, int line) {
  return newoI(p_this, n * size, file, line);
}

void deleteoI(void **p_this, ulong bytes, char *file, int line) {
  void *this;

  if (p_this == null) {
    throw_("Pointer to pointer null");
  }

  if ((this = *p_this) == null) {
    return;
  }

  free(this);
  *p_this = null;
}

void deleteI(void **p_this,
    ulong n,
    ulong (*destroy)(void **p_this),
    ulong bytes,
    char *file,
    int line) {
  void **p;
  void **e;
  void *this;

  if (destroy == null) {
    throw_("Destructor null");
  }

  if (p_this == null) {
    throw_("Pointer to pointer null");
  }

  if ((this = *p_this) == null) {
    return;
  }

  for (p = (void **)this, e = p + n; p < e; p++) {
    if (*p != null) {
      destroy(p);
    }
  }

  deleteoI(p_this, n * bytes, file, line);
}

void renewI(void **p_this,
    ulong old_n,
    ulong new_n,
    ulong bytes,
    ulong (*destroy)(void **p_this),
    char *file,
    int line) {
  void **p;
  void **e;
  ulong new_bytes;
  ulong old_bytes;
  void *this;

  if (p_this == null) {
    throw_("Pointer to pointer null");
  }

  if ((this = *p_this) == null) {
    throw_("Pointer empty");
  }

  if (new_n == old_n) {
    return;
  }

  if (new_n < old_n && destroy != null) {
    for (p = (void **)this + new_n, e = (void **)this + old_n; p < e; p++) {
      if (*p != null) {
        destroy(p);
      }
    }
  }

  old_bytes = old_n * bytes;

  if (old_bytes == 0) {
    old_bytes = 1;
  }

  new_bytes = new_n * bytes;

  if (new_bytes == 0) {
    new_bytes = 1;
  }

  if ((this = *p_this = realloc(this, new_bytes > 0 ? new_bytes : 1)) == null) {
    throw_("Out of memory reallocating %lu -> %lu bytes",
        old_n * bytes,
        new_bytes);
  }

  if (new_n > old_n) {
    memset((byte *)this + old_bytes, 0, new_bytes - old_bytes);
  }
}

void renewupI(void **p_this,
    ulong old_n,
    ulong new_n,
    ulong bytes,
    char *file,
    int line) {
  if (new_n < old_n) {
    throw_("Decrease from %lu to %lu!", old_n, new_n);
  }

  renewI(p_this, old_n, new_n, bytes, null, file, line);
}

void newmI(void ***p_this, ulong n0, ulong bytes, char *file, int line) {
  NewMPass_ pass;
  void **this;

  newI((void **)p_this, n0, sizeof(void *), file, line);
  this = *p_this;

  pass.bytes = bytes;
  pass.file = file;
  pass.line = line;
  pass.this = this;

  parallel_for(n0, MIN_ / maximum(bytes, 1), newm_, &pass);
}

void newrmI(void ***p_this,
    ulong n0,
    ulong *n1s,
    ulong size,
    char *file,
    int line) {
  ulong bytes;
  ulong u;
  NewRMPass_ pass;
  void **this;

  newI((void **)p_this, n0, sizeof(void *), file, line);
  this = *p_this;

  pass.size = size;
  pass.n1s = n1s;
  pass.file = file;
  pass.line = line;
  pass.this = this;

  for (u = 0, bytes = 0; u < n0; u++) {
    bytes += size * n1s[u];
  }

  parallel_for(n0, MIN_ * n0 / maximum(bytes, 1), newrm_, &pass);
}

void deletemI(void ***p_this,
    ulong n0,
    ulong n1,
    ulong (*destroy)(void **p_this),
    ulong size,
    char *file,
    int line) {
  DeleteMPass_ pass;
  void **this;

  if (p_this == null) {
    throw_("Pointer to pointer null");
  }

  if ((this = *p_this) == null) {
    return;
  }

  pass.n1 = n1;
  pass.size = size;
  pass.file = file;
  pass.line = line;
  pass.destroy = destroy;
  pass.this = this;

  parallel_for(n0, MIN_ / maximum(n1 * size, 1), deletem_, &pass);

  deleteoI((void **)p_this, n0 * sizeof(void **), file, line);
}

void deletermI(void ***p_this,
    ulong n0,
    ulong *n1s,
    ulong (*destroy)(void **p_this),
    ulong size,
    char *file,
    int line) {
  ulong bytes;
  ulong u;
  DeleteRMPass_ pass;
  void **this;

  if (p_this == null) {
    throw_("Pointer to pointer null");
  }

  if ((this = *p_this) == null) {
    return;
  }

  pass.n1s = n1s;
  pass.size = size;
  pass.file = file;
  pass.line = line;
  pass.destroy = destroy;
  pass.this = this;

  for (u = 0, bytes = 0; u < n0; u++) {
    bytes += size * n1s[u];
  }

  parallel_for(n0, MIN_ * n0 / maximum(bytes, 1), deleterm_, &pass);

  deleteoI((void **)p_this, n0 * sizeof(void **), file, line);
}

void deletemvI(void ***p_this,
    ulong n0,
    char *file,
    int line) {
  DeleteMVPass_ pass;
  void **this;

  if (p_this == null) {
    throw_("Pointer to pointer null");
  }

  if ((this = *p_this) == null) {
    return;
  }

  pass.file = file;
  pass.line = line;
  pass.this = this;

  parallel_for(n0, MIN_ / 8, deletemv_, &pass);

  deleteoI((void **)p_this, n0 * sizeof(void **), file, line);
}

void deletermvI(void ***p_this,
    ulong n0,
    ulong *n1s,
    ulong size,
    char *file,
    int line) {
  ulong bytes;
  ulong u;
  DeleteRMVPass_ pass;
  void **this;

  if (p_this == null) {
    throw_("Pointer to pointer null");
  }

  if ((this = *p_this) == null) {
    return;
  }

  pass.size = size;
  pass.n1s = n1s;
  pass.file = file;
  pass.line = line;
  pass.this = this;

  for (u = 0, bytes = 0; u < n0; u++) {
    bytes += size * n1s[u];
  }

  parallel_for(n0, MIN_ * n0 / maximum(bytes, 1), deletermv_, &pass);

  deleteoI((void **)p_this, n0 * sizeof(void **), file, line);
}

void newt3I(void ****p_this,
    ulong n0,
    ulong n1,
    ulong bytes,
    char *file,
    int line) {
  NewT3Pass_ pass;
  void ***this;

  newI((void **)p_this, n0, sizeof(void **), file, line);
  this = *p_this;

  pass.n1 = n1;
  pass.bytes = bytes;
  pass.file = file;
  pass.line = line;
  pass.this = this;

  parallel_for(n0, MIN_ / maximum(n1 * bytes, 1), newt3_, &pass);
}

void deletet3I(void ****p_this,
    ulong n0,
    ulong n1,
    ulong n2,
    ulong (*destroy)(void **p_this),
    ulong bytes,
    char *file,
    int line) {
  DeleteT3Pass_ pass;
  void ***this;

  if (p_this == null) {
    throw_("Pointer to pointer null");
  }

  if ((this = *p_this) == null) {
    return;
  }

  pass.n1 = n1;
  pass.n2 = n2;
  pass.bytes = bytes;
  pass.file = file;
  pass.line = line;
  pass.destroy = destroy;
  pass.this = this;

  parallel_for(n0, MIN_ / maximum(n1 * n2 * bytes, 1), deletet3_, &pass);

  deleteoI((void **)p_this, n0 * sizeof(void **), file, line);
}

void deletet3vI(
    void ****p_this,
    ulong n0,
    ulong n1,
    char *file,
    int line) {
  DeleteT3VPass_ pass;
  void ***this;

  if (p_this == null) {
    throw_("Pointer to pointer null");
  }

  if ((this = *p_this) == null) {
    return;
  }

  pass.n1 = n1;
  pass.file = file;
  pass.line = line;
  pass.this = this;

  parallel_for(n0, MIN_ / maximum(n1 * 8, 1), deletet3v_, &pass);

  deleteoI((void **)p_this, n0 * sizeof(void **), file, line);
}

void newt4I(void *****p_this,
    ulong n0,
    ulong n1,
    ulong n2,
    ulong bytes,
    char *file,
    int line) {
  NewT4Pass_ pass;
  void ****this;

  newI((void **)p_this, n0, sizeof(void **), file, line);
  this = *p_this;

  pass.n1 = n1;
  pass.n2 = n2;
  pass.bytes = bytes;
  pass.file = file;
  pass.line = line;
  pass.this = this;

  parallel_for(n0, MIN_ / maximum(n1 * n2 * bytes, 1), newt4_, &pass);
}

void deletet4I(void *****p_this,
    ulong n0,
    ulong n1,
    ulong n2,
    ulong n3,
    ulong (*destroy)(void **p_this),
    ulong bytes,
    char *file,
    int line) {
  DeleteT4Pass_ pass;
  void ****this;

  if (p_this == null) {
    throw_("Pointer to pointer null");
  }

  if ((this = *p_this) == null) {
    return;
  }

  pass.n1 = n1;
  pass.n2 = n2;
  pass.n3 = n3;
  pass.bytes = bytes;
  pass.file = file;
  pass.line = line;
  pass.destroy = destroy;
  pass.this = this;

  parallel_for(n0, MIN_ / maximum(n1 * n2 * n3 * bytes, 1), deletet4_, &pass);

  deleteoI((void **)p_this, n0 * sizeof(void **), file, line);
}

void deletet4vI(
    void *****p_this,
    ulong n0,
    ulong n1,
    ulong n2,
    char *file,
    int line) {
  DeleteT4VPass_ pass;
  void ****this;

  if (p_this == null) {
    throw_("Pointer to pointer null");
  }

  if ((this = *p_this) == null) {
    return;
  }

  pass.n1 = n1;
  pass.n2 = n2;
  pass.file = file;
  pass.line = line;
  pass.this = this;

  parallel_for(n0, MIN_ / maximum(n1 * n2 * 8, 1), deletet4v_, &pass);

  deleteoI((void **)p_this, n0 * sizeof(void **), file, line);
}

void donateI(void **psrc, void **pdst, char *file, int line) {
  if (psrc == null) {
    throw_("Pointer to source pointer null");
  }

  if (pdst == null) {
    throw_("Pointer to destination pointer null");
  }

  if (*psrc == null) {
    throw_("Source pointer empty");
  }

  if (*pdst != null) {
    throw_("Destination pointer not empty");
  }

  *pdst = *psrc;
  *psrc = null;
}

void throw_lockI(void) {
  lock(&throw_lock_);
}

void throw_unlockI(void) {
  unlock(&throw_lock_);
}

void throwI(char *fmt, ...) {
  char *_space = null;
  va_list ap1;
  va_list ap2;

  if (g_JPC_try_ == 2 && !g_JPC_internal_throw_) {
    string_renew_copy(&g_JPC_orig_exception_, exception);
  }

  string_delete(&exception);
  va_start(ap1, fmt);
  va_start(ap2, fmt);
  string_new_vf(&exception, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);

  string_renew_escape_for_logging(&exception);

  if (g_JPC_exception_separators_) {
    string_renew_add_separators(&exception);
  }

  if (g_JPC_try_ == 1) {
    g_JPC_try_ = 2;
    throw_unlockI();
    longjmp(g_JPC_jmp_buf_, 1);
  }

  string_new_copy(&_space, string_ends_with(exception, "\n", false) ? "" : " ");
  fprintf(stderr,
      "\nERROR: %s%s(%s:%d)\n\n",
      exception,
      _space,
      g_JPC_file_1_,
      g_JPC_line_1_);
  string_delete(&_space);

  if (g_JPC_try_ == 2) {
    string_renew_escape_for_logging(&g_JPC_orig_exception_);

    if (g_JPC_exception_separators_) {
      string_renew_add_separators(&g_JPC_orig_exception_);
    }

    string_new_copy(
        &_space,
        string_ends_with(g_JPC_orig_exception_, "\n", false) ? "" : " ");
    fprintf(stderr,
        "RETHROWN AFTER: %s%s(%s:%d)\n\n",
        g_JPC_orig_exception_,
        _space,
        g_JPC_file_2_,
        g_JPC_line_2_);
    string_delete(&_space);
  }

  throw_unlockI();

#ifdef JPC_DEBUG
  /* Write to address zero to leverage ASAN's printing of the stack trace.
   */
  *(volatile char *)null = '\0';
#endif

  exit(1);
}

void exit_normally(void) {
  if (allow_exit_normally_) {
    exit(0);
  }

  throw("exit_normally() was called, but this has been disallowed");
}

void allow_exit_normally(bool allow) {
  allow_exit_normally_ = allow;
}

double round(double x) {
  return x < 0 ? ceil(x - 0.5) : floor(x + 0.5);
}

void defrag_pointer_arrayI(void ***p_this, ulong *pn) {
  void **a;
  void **b;
  void **e;
  ulong n;
  void **this;

  this = *p_this;
  n = *pn;

  for (a = this, b = a, e = b + n; b < e; b++) {
    if (*b != null) {
      *a++ = *b;
    }
  }

  n = a - this;
  renew(p_this, *pn, n, null);
  *pn = n;
}

void copy_memory(void *src, void *dst, ulong num_bytes) {
  CopyPass_ pass;

  if (labs((byte *)dst - (byte *)src) < num_bytes) {
    throw("Attempting to copy memory between overlapping ranges: "
        "src [%p to %p] dst [%p to %p]",
        src,
        (byte *)src + num_bytes - 1,
        dst,
        (byte *)dst + num_bytes - 1);
  }

  pass.src = src;
  pass.dst = dst;
  parallel_for(num_bytes, MIN_, copy_, &pass);
}

void copy_memorym(void *src, void *dst, ulong n0, ulong num_bytes) {
  CopyMPass_ pass;

  pass.src = src;
  pass.dst = dst;
  pass.numBytes = num_bytes;
  parallel_for(n0, MIN_ / maximum(num_bytes, 1), copym_, &pass);
}

void copy_memorym_flatten(void *src, void *dst, ulong n0, ulong num_bytes) {
  CopyMPass_ pass;

  pass.src = src;
  pass.dst = dst;
  pass.numBytes = num_bytes;
  parallel_for(n0, MIN_ / maximum(num_bytes, 1), copym_flatten_, &pass);
}

void copy_memorym_unflatten(void *src, void *dst, ulong n0, ulong num_bytes) {
  CopyMPass_ pass;

  pass.src = src;
  pass.dst = dst;
  pass.numBytes = num_bytes;
  parallel_for(n0, MIN_ / maximum(num_bytes, 1), copym_unflatten_, &pass);
}

void clear_memory(void *memory, ulong num_bytes) {
  parallel_for(num_bytes, MIN_, clear_, memory);
}

void move_memory(void *src, void *dst, ulong n) {
  copy_memory(src, dst, n);
  clear_memory(src, n);
}

bool memory_equals(void *a, void *b, ulong num_bytes) {
  bool equals;
  EqualsPass_ pass;

  equals = true;
  pass.a = a;
  pass.b = b;
  pass.equals = &equals;
  parallel_for(num_bytes, MIN_, equals_, &pass);

  return equals;
}

bool values_equalmI(byte **a, byte **b, ulong n0, ulong num_bytes) {
  bool equals;
  EqualsMPass_ pass;

  equals = true;
  pass.a = a;
  pass.b = b;
  pass.numBytes = num_bytes;
  pass.equals = &equals;
  parallel_for(n0, MIN_, equalsm_, &pass);

  return equals;
}

bool little_endian(void) {
  static bool initialized__ = false;
  static bool is__ = false;
  static Lock lock__ = LOCK_INITIALIZER;

  union {
    byte b[8];
    ulong u;
  } c;

  if (!initialized__) {
    lock(&lock__);

    if (!initialized__) {
      if (sizeof(c) != 8) {
        throw("Impossible");
      }

      c.u = 0x0123456789abcdef;

      if (c.b[0] == 0xef && c.b[1] == 0xcd && c.b[2] == 0xab
        && c.b[3] == 0x89 && c.b[4] == 0x67 && c.b[5] == 0x45
        && c.b[6] == 0x23 && c.b[7] == 0x01) {
        is__ = true;
      } else if (c.b[7] == 0xef && c.b[6] == 0xcd && c.b[5] == 0xab
        && c.b[4] == 0x89 && c.b[3] == 0x67 && c.b[2] == 0x45
        && c.b[1] == 0x23 && c.b[0] == 0x01) {
        is__ = false;
      } else {
        throw("Impossible: Could not parse endiannness");
      }

      initialized__ = true;
    }

    unlock(&lock__);
  }

  return is__;
}

void useI(void *p) {
}

void delete_nothing(void **p_this) {
}

sbyte pointer_compare(void *a, void *b, void *x) {
  if (a < b) {
    return -1;
  }

  if (a > b) {
    return +1;
  }

  return 0;
}

sbyte pointer_pointer_compare(void *p_a, void *p_b, void *x) {
  return pointer_compare(*(void **)p_a, *(void **)p_b, x);
}


static void newm_(Parallel *par) {
  ulong bytes;
  ulong u;
  NewMPass_ *pass;
  void **this;

  pass = par->x;
  bytes = pass->bytes;
  this = pass->this;

  for (u = par->start; u < par->end; u++) {
    newI(&this[u], bytes, 1, pass->file, pass->line);
  }
}

static void newrm_(Parallel *par) {
  ulong size;
  ulong u;
  ulong *n1s;
  NewRMPass_ *pass;
  void **this;

  pass = par->x;
  size = pass->size;
  n1s = pass->n1s;
  this = pass->this;

  for (u = par->start; u < par->end; u++) {
    newI(&this[u], n1s[u], size, pass->file, pass->line);
  }
}

static void deletem_(Parallel *par) {
  ulong n1;
  ulong size;
  ulong u;
  DeleteMPass_ *pass;
  ulong (*destroy)(void **p_this);
  void **this;

  pass = par->x;
  n1 = pass->n1;
  size = pass->size;
  this = pass->this;
  destroy = pass->destroy;

  for (u = par->start; u < par->end; u++) {
    deleteI(&this[u], n1, destroy, size, pass->file, pass->line);
  }
}

static void deleterm_(Parallel *par) {
  ulong size;
  ulong u;
  ulong *n1s;
  DeleteRMPass_ *pass;
  ulong (*destroy)(void **p_this);
  void **this;

  pass = par->x;
  n1s = pass->n1s;
  size = pass->size;
  this = pass->this;
  destroy = pass->destroy;

  for (u = par->start; u < par->end; u++) {
    deleteI(&this[u], n1s[u], destroy, size, pass->file, pass->line);
  }
}

static void deletemv_(Parallel *par) {
  ulong bytes;
  ulong u;
  DeleteMVPass_ *pass;
  void **this;

  pass = par->x;
  bytes = pass->bytes;
  this = pass->this;

  for (u = par->start; u < par->end; u++) {
    deleteoI(&this[u], bytes, pass->file, pass->line);
  }
}

static void deletermv_(Parallel *par) {
  ulong size;
  ulong u;
  ulong *n1s;
  DeleteRMVPass_ *pass;
  void **this;

  pass = par->x;
  size = pass->size;
  n1s = pass->n1s;
  this = pass->this;

  for (u = par->start; u < par->end; u++) {
    deleteoI(&this[u], size * n1s[u], pass->file, pass->line);
  }
}

static void newt3_(Parallel *par) {
  ulong bytes;
  ulong n1;
  ulong u;
  NewT3Pass_ *pass;
  void ***this;

  pass = par->x;
  bytes = pass->bytes;
  n1 = pass->n1;
  this = pass->this;

  for (u = par->start; u < par->end; u++) {
    newmI(&this[u], n1, bytes, pass->file, pass->line);
  }
}

static void deletet3_(Parallel *par) {
  ulong n1;
  ulong n2;
  ulong bytes;
  ulong u;
  DeleteT3Pass_ *pass;
  ulong (*destroy)(void **p_this);
  void ***this;

  pass = par->x;
  n1 = pass->n1;
  n2 = pass->n2;
  bytes = pass->bytes;
  this = pass->this;
  destroy = pass->destroy;

  for (u = par->start; u < par->end; u++) {
    deletemI(&this[u], n1, n2, destroy, bytes, pass->file, pass->line);
  }
}

static void deletet3v_(Parallel *par) {
  ulong n1;
  ulong u;
  DeleteT3VPass_ *pass;
  void ***this;

  pass = par->x;
  n1 = pass->n1;
  this = pass->this;

  for (u = par->start; u < par->end; u++) {
    deletemvI(&this[u], n1, pass->file, pass->line);
  }
}

static void newt4_(Parallel *par) {
  ulong bytes;
  ulong n1;
  ulong n2;
  ulong u;
  NewT4Pass_ *pass;
  void ****this;

  pass = par->x;
  bytes = pass->bytes;
  n1 = pass->n1;
  n2 = pass->n2;
  this = pass->this;

  for (u = par->start; u < par->end; u++) {
    newt3I(&this[u], n1, n2, bytes, pass->file, pass->line);
  }
}

static void deletet4_(Parallel *par) {
  ulong n1;
  ulong n2;
  ulong n3;
  ulong bytes;
  ulong u;
  DeleteT4Pass_ *pass;
  ulong (*destroy)(void **p_this);
  void ****this;

  pass = par->x;
  n1 = pass->n1;
  n2 = pass->n2;
  n3 = pass->n3;
  bytes = pass->bytes;
  this = pass->this;
  destroy = pass->destroy;

  for (u = par->start; u < par->end; u++) {
    deletet3I(&this[u],
        n1,
        n2,
        n3,
        destroy,
        bytes,
        pass->file,
        pass->line);
  }
}

static void deletet4v_(Parallel *par) {
  ulong n1;
  ulong n2;
  ulong u;
  DeleteT4VPass_ *pass;
  void ****this;

  pass = par->x;
  n1 = pass->n1;
  n2 = pass->n2;
  this = pass->this;

  for (u = par->start; u < par->end; u++) {
    deletet3vI(&this[u], n1, n2, pass->file, pass->line);
  }
}

static void copy_(Parallel *par) {
  CopyPass_ *pass;

  pass = par->x;

  memcpy(pass->dst + par->start, pass->src + par->start, par->end - par->start);
}

static void copym_(Parallel *par) {
  ulong n0;
  CopyMPass_ *pass;

  pass = par->x;

  for (n0 = par->start; n0 < par->end; n0++) {
    copy_memory(pass->src[n0], pass->dst[n0], pass->numBytes);
  }
}

static void copym_flatten_(Parallel *par) {
  byte *dst;
  ulong n0;
  ulong num_bytes;
  CopyMPass_ *pass;

  pass = par->x;
  num_bytes = pass->numBytes;

  for (n0 = par->start, dst = (byte *)pass->dst + n0 * num_bytes;
      n0 < par->end;
      n0++, dst += num_bytes) {
    copy_memory(pass->src[n0], dst, num_bytes);
  }
}

static void copym_unflatten_(Parallel *par) {
  byte *src;
  ulong n0;
  ulong num_bytes;
  CopyMPass_ *pass;

  pass = par->x;
  num_bytes = pass->numBytes;

  for (n0 = par->start, src = (byte *)pass->src + n0 * num_bytes;
      n0 < par->end;
      n0++, src += num_bytes) {
    copy_memory(src, pass->dst[n0], num_bytes);
  }
}

static void clear_(Parallel *par) {
  memset((byte *)par->x + par->start, 0, par->end - par->start);
}

static void equals_(Parallel *par) {
  bool *equals;
  byte *a;
  byte *b;
  ulong u;
  EqualsPass_ *pass;

  pass = par->x;
  equals = pass->equals;
  a = pass->a;
  b = pass->b;

  for (u = par->start; *equals && u < par->end; u++) {
    if (a[u] != b[u]) {
      *equals = false;
    }
  }
}

static void equalsm_(Parallel *par) {
  bool *equals;
  byte **a;
  byte **b;
  ulong n0;
  ulong num_bytes;
  EqualsMPass_ *pass;

  pass = par->x;
  equals = pass->equals;
  a = pass->a;
  b = pass->b;
  num_bytes = pass->numBytes;

  for (n0 = par->start; *equals && n0 < par->end; n0++) {
    if (!memory_equals(a[n0], b[n0], num_bytes)) {
      *equals = false;
    }
  }
}
