#include "shuffle.h"

#define MIN_ 100000

typedef struct {
  ulong index;
  ulong random;
} Lookup_;

typedef struct {
  byte *src;
  byte *dst;
  ulong size;
  Lookup_ *lookups;
} Pass_;

static void setup_(Parallel *par);

static void shuffle_(Parallel *par);

static sbyte compare_(Lookup_ *a, Lookup_ *b, void *x);


void shuffleI(void *base, ulong n, ulong size) {
  byte *_copy = null;
  Lookup_ *_lookups = null;
  Pass_ pass;

  new(&_copy, n * size);
  copy_memory(base, _copy, n * size);
  new(&_lookups, n);
  pass.src = _copy;
  pass.dst = base;
  pass.size = size;
  pass.lookups = _lookups;
  parallel_for(n, MIN_, setup_, &pass);
  qsortx(_lookups, n, compare_, false, null);
  parallel_for(n, MIN_, shuffle_, &pass);
  deletev(&_lookups);
  deletev(&_copy);
}

void shuffle_make_lookups_and_inverses(
    ulong **plookups,
    ulong **pinverses,
    ulong n) {
  ulong u;
  ulong *lookups;
  ulong *inverses;

  shuffle_make_lookups(plookups, n);
  lookups = *plookups;

  new(pinverses, n);
  inverses = *pinverses;

  for (u = 0; u < n; u++) {
    inverses[lookups[u]] = u;
  }
}

void shuffle_make_lookups(ulong **plookups, ulong n) {
  ulong u;
  ulong *lookups;

  new(plookups, n);
  lookups = *plookups;

  for (u = 0; u < n; u++) {
    lookups[u] = u;
  }

  shuffle(lookups, n);
}

static void setup_(Parallel *par) {
  ulong u;
  ulong end;
  Pass_ *pass;
  Lookup_ *lookup;
  Random *_random = null;

  pass = par->x;

  random_new_offset(&_random, par->t);

  for (u = par->start, end = par->end; u < end; u++) {
    lookup = &pass->lookups[u];
    lookup->index = u;
    lookup->random = random_ulong(_random);
  }

  random_delete(&_random);
}

static void shuffle_(Parallel *par) {
  byte *src;
  byte *dst;
  ulong size;
  ulong u;
  Pass_ *pass;
  Lookup_ *lookups;

  pass = par->x;

  src = pass->src;
  dst = pass->dst;
  size = pass->size;
  lookups = pass->lookups;

  for (u = par->start; u < par->end; u++) {
    copy_memory(src + u * size, dst + lookups[u].index * size, size);
  }
}

static sbyte compare_(Lookup_ *a, Lookup_ *b, void *x) {
  ulong ra;
  ulong rb;

  ra = a->random;
  rb = b->random;

  return ra < rb ? -1 : ra > rb ? +1 : 0;
}
