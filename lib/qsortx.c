#include "qsortx.h"

#define MIN_ 10000
#define MAX_THRESH_ 4
#define STACK_SIZE_ 64

#define PUSH_(M_l_, M_h_) top->lo = (M_l_); top->hi = (M_h_); top++;

#define POP_(M_l_, M_h_) top--; M_l_ = top->lo; M_h_ = top->hi;

#define min_(x, y) ((x) < (y) ? (x) : (y))

#define SWAP_(a, b) { \
  byte M_tmp_; \
  byte *M_a_; \
  byte *M_b_; \
  ulong M_size_; \
  \
  M_size_ = size; \
  M_a_ = (a); \
  M_b_ = (b); \
  \
  do { \
    M_tmp_ = *M_a_; \
    *M_a_++ = *M_b_; \
    *M_b_++ = M_tmp_; \
  } while (--M_size_ > 0); \
}

typedef struct {
  byte *base1;
  byte *base2;
  byte *base3;
  ulong n1;
  ulong n2;
} Q_;

typedef struct {
  byte *base;
  ulong size;
  sbyte (*compare)(void *a, void *b, void *x);
  void *x;
  Q_ *_qs;
} Pass_;

typedef struct {
  byte *lo;
  byte *hi;
} Node_;

static void qsortx_(void *base,
    ulong n,
    ulong size,
    sbyte (*compare)(void *a, void *b, void *x),
    void *x);

static void init_(Parallel *par);

static void sort1_(Parallel *par);

static void sort2_(Parallel *par);

static sbyte compare_(void *a,
    void *z,
    void *x,
    byte *b,
    byte *e,
    sbyte (*compare)(void *a, void *z, void *x));


void qsortxI(void *base,
    ulong n,
    ulong size,
    sbyte (*compare)(void *a, void *b, void *x),
    bool shuffle_equal,
    void *x) {
  byte *a;
  byte *_scratch = null;
  byte *tmp;
  byte *src_base;
  byte *dst_base;
  ulong first;
  ulong last;
  ulong num_threads;
  ulong u;
  ulong v;
  ulong w;
  Q_ *q;
  Q_ *r;
  Pass_ pass;

  if (size == 0) {
    throw("Size of each element cannot be zero");
  }

  num_threads = parallel_num_threads(n, MIN_);
  pass.base = base;
  pass.size = size;
  pass._qs = null;
  new(&pass._qs, num_threads);
  parallel_for_specify(n, num_threads, init_, &pass);

  pass.compare = compare;
  pass.x = x;
  parallel_for_specify(n, num_threads, sort1_, &pass);

  new(&_scratch, n * size);

  src_base = base;
  dst_base = _scratch;

  for (w = num_threads; w > 1; w = v) {
    for (u = 0, v = 0; u < w; u += 2, v++) {
      q = &pass._qs[v];
      r = &pass._qs[u];
      q->base1 = r->base3;
      q->base3 = q->base1 - src_base + dst_base;
      q->n1 = r->n1 + r->n2;

      if (u + 1 == w) {
        q->base2 = null;
        q->n2 = 0;
      } else {
        r = &pass._qs[u + 1];
        q->base2 = r->base3;
        q->n2 = r->n1 + r->n2;
      }
    }

    parallel_for_specify(v, v, sort2_, &pass);

    tmp = src_base;
    src_base = dst_base;
    dst_base = tmp;
  }

  if (src_base != base) {
    copy_memory(_scratch, base, n * size);
  }

  deletev(&_scratch);
  deletev(&pass._qs);

  if (shuffle_equal) {
    for (first = 0; first < n; first = last + 1) {
      a = (byte *)base + first * size;

      for (
          last = first + 1;
          last < n && compare(a, (byte *)base + last * size, x) == 0;
          last++) {
      }

      last--;

      if (last > first) {
        shuffleI(a, last - first + 1, size);
      }
    }
  }
}


static void init_(Parallel *par) {
  Q_ *q;
  Pass_ *pass;

  pass = par->x;
  q = &pass->_qs[par->t];

  q->base1 = q->base3 = pass->base + par->start * pass->size;
  q->n1 = par->end - par->start;
  q->n2 = 0;
}

static void sort1_(Parallel *par) {
  Q_ *q;
  Pass_ *pass;

  pass = par->x;
  q = &pass->_qs[par->t];

  qsortx_(q->base1, q->n1, pass->size, pass->compare, pass->x);
}

static void sort2_(Parallel *par) {
  byte *p1;
  byte *e1;
  byte *p2;
  byte *e2;
  byte *p3;
  byte *p;
  sbyte c;
  Q_ *q;
  Pass_ *pass;

  pass = par->x;
  q = &pass->_qs[par->t];

  if (q->n2 == 0) {
    copy_memory(q->base1, q->base3, q->n1 * pass->size);
  } else {
    for (p1 = q->base1,
        e1 = p1 + q->n1 * pass->size,
        p2 = q->base2,
        e2 = p2 + q->n2 * pass->size,
        p3 = q->base3;
        p1 < e1 || p2 < e2;
        p3 += pass->size) {
      if (p1 == e1) {
        p = p2;
        p2 += pass->size;
      } else if (p2 == e2) {
        p = p1;
        p1 += pass->size;
      } else {
        c = pass->compare(p1, p2, pass->x);

        if (c <= 0) {
          p = p1;
          p1 += pass->size;
        } else {
          p = p2;
          p2 += pass->size;
        }
      }

      copy_memory(p, p3, pass->size);
    }

    if (p3 - q->base3 != (q->n1 + q->n2) * pass->size) {
      throw("Impossible");
    }
  }
}

static void qsortx_(void *base,
    ulong n,
    ulong size,
    sbyte (*compare)(void *a, void *b, void *x),
    void *x) {
  byte c;
  byte *b;
  byte *lo;
  byte *hi;
  byte *l;
  byte *m;
  byte *r;
  byte *e;
  byte *t;
  byte *thresh;
  byte *run;
  byte *trav;
  ulong max_thresh;
  Node_ *top;

  if (size == 0) {
    throw("Size of each element cannot be zero");
  }

  if (n < 2) {
    return;
  }

  b = (byte *)base;
  e = &b[size * (n - 1)];
  max_thresh = MAX_THRESH_ * size;

  if (n > MAX_THRESH_) {
    Node_ stack[STACK_SIZE_];

    lo = b;
    hi = &lo[size * (n - 1)];
    top = stack;

    PUSH_(null, null);

    while (stack < top) {
      m = lo + size * ((hi - lo) / size >> 1);

      if (compare_(m, lo, x, b, e, compare) < 0) {
        SWAP_(m, lo);
      }

      if (compare_(hi, m, x, b, e, compare) < 0) {
        SWAP_(m, hi);

        if (compare_(m, lo, x, b, e, compare) < 0) {
          SWAP_(m, lo);
        }
      }

      l = lo + size;
      r = hi - size;

      do {
        while (compare_(l, m, x, b, e, compare) < 0) {
          l += size;
        }

        while (compare_(m, r, x, b, e, compare) < 0) {
          r -= size;
        }

        if (l < r) {
          SWAP_(l, r);

          if (m == l) {
            m = r;
          } else if (m == r) {
            m = l;
          }

          l += size;
          r -= size;
        }
        else if (l == r) {
          l += size;
          r -= size;
          break;
        }
      } while (l <= r);

      if (r - lo <= max_thresh) {
        if (hi - l <= max_thresh) {
          POP_(lo, hi);
        } else {
          lo = l;
        }
      }
      else if (hi - l <= max_thresh) {
        hi = r;
      } else if (r - lo > hi - l) {
        PUSH_(lo, r);
        lo = l;
      } else {
        PUSH_(l, hi);
        hi = r;
      }
    }
  }

  t = b;
  thresh = min_(e, b + max_thresh);

  for (run = t + size; run <= thresh; run += size) {
    if (compare_(run, t, x, b, e, compare) < 0) {
      t = run;
    }
  }

  if (t != b) {
    SWAP_(t, b);
  }

  run = b + size;

  while ((run += size) <= e) {
    t = run - size;

    while (compare_(run, t, x, b, e, compare) < 0) {
      t -= size;
    }

    t += size;

    if (t != run) {
      trav = run + size;

      while (--trav >= run) {
        c = *trav;

        for (hi = lo = trav; (lo -= size) >= t; hi = lo) {
          *hi = *lo;
        }

        *hi = c;
      }
    }
  }
}

void *bsearchxI(
    void *key,
    void *base,
    ulong n,
    ulong size,
    sbyte (*compare)(void *key, void *element, void *x),
    void *x) {
  byte *b;
  byte *p;
  sbyte c;

  for (b = base; n != 0; n >>= 1) {
    p = b + (n >> 1) * size;
    c = compare(key, p, x);

    if (c == 0) {
      return p;
    }

    if (c > 0) {
      b = p + size;
      n--;
    }
  }

  return null;
}

ulong bsearchx_find_indexI(
    void *key,
    void *base,
    ulong n,
    ulong size,
    sbyte (*compare)(void *key, void *element, void *x),
    void *x) {
  ulong index = 0;

  if (!bsearchx_try_find_indexI(key, base, n, size, compare, x, &index)) {
    throw("Could not find element");
  }

  return index;
}

bool bsearchx_try_find_indexI(
    void *key,
    void *base,
    ulong n,
    ulong size,
    sbyte (*compare)(void *a, void *b, void *x),
    void *x,
    ulong *pindex) {
  void *p;

  p = bsearchxI(key, base, n, size, compare, x);

  if (p == null) {
    return false;
  }

  *pindex = ((byte *)p - (byte *)base) / size;

  return true;
}

static sbyte compare_(void *a,
    void *z,
    void *x,
    byte *b,
    byte *e,
    sbyte (*compare)(void *a, void *z, void *x)) {
  if ((byte *)a < b) {
    throw("Impossible: a (%p) to left of first (%p); comparator must be bad",
        a,
        b);
  }

  if ((byte *)a > e) {
    throw("Impossible: a (%p) to right of last (%p); comparator must be bad",
        a,
        e);
  }

  if ((byte *)z < b) {
    throw("Impossible: b (%p) to left of first (%p); comparator must be bad",
        z,
        b);
  }

  if ((byte *)z > e) {
    throw("Impossible: b (%p) to right of last (%p); comparator must be bad",
        z,
        e);
  }

  return compare(a, z, x);
}
