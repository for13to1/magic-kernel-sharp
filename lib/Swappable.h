/* Static factory plus object class for allocating memory that is automatically
 * swapped out to storage when the memory used by all swappable blocks of
 * memory that are currently in-memory exceeds the specified threshold.
 */
#ifndef SWAPPABLE_H
#define SWAPPABLE_H

#ifdef __cplusplus
extern "C" {
#endif

struct SwappableValues;

#include "DynamicReferenceSet.h"
#include "Lru.h"
#include "Progress.h"
#include "TempFilename.h"
#include "memory.h"

typedef struct SwappableValues {
  byte *_bytes;
  char *_name;
  ulong numBytes;
  ulong refCount;
  struct TempFilename *_tempFilename;
} SwappableValues;

typedef struct {
  ulong refCount;
  Lock lock;
  void **po;
  struct File *_file;
  void (*saver)(void **this, struct File *f);
  ulong (*destroy)(void **this);
  ulong (*loader)(void **this, struct File *f);
} Swappable;

/* Constructor.
 */
#define swappable_values_new(p_this, cls, n) \
  swappable_values_newI((p_this), (n) * sizeof(cls), #p_this);

void swappable_values_newI(
    SwappableValues **p_this,
    ulong num_bytes,
    char *name);

/* Access constructor.
 */
#define swappable_values_new_access(p_this, cls, n, po) \
  swappable_values_new((p_this), cls, (n)); \
  swappable_values_access(*(p_this), (po));

/* Constructor for an array of of swappables, i.e., a matrix of data.
 */
#define swappable_values_newm(p_this, cls, n0, n1) \
  swappable_values_newmI((p_this), (n0), (n1) * sizeof(cls), #p_this);

void swappable_values_newmI(
    SwappableValues ***p_this,
    ulong n0,
    ulong num_bytes,
    char *name);

/* Access constructor for an array of of swappables, i.e., a matrix of data.
 */
#define swappable_values_newm_access(p_this, cls, n0, n1, ppo) \
  swappable_values_newm((p_this), cls, (n0), (n1)); \
  swappable_values_accessm(*(p_this), (n0), (ppo));

/* Destructor.
 */
void swappable_values_delete(SwappableValues **p_this);

/* Destructor for an array of swappables, i.e., a matrix of data.
 */
#define swappable_values_deletem(p_this, n0) \
  delete((p_this), (n0), swappable_values_delete)

/* Print.
 */
void swappable_values_print(SwappableValues *this, struct Printer *p);

/* Access a swappable by getting a pointer to its memory. This automatically
 * swaps the memory back in from storage if it is currently swapped out. The
 * pointer to the desired empty memory pointer must be passed in. This pointer
 * remains valid until swappable_release() is called. Until that time, this
 * object will definitely not be swapped out to storage.
 */
void swappable_values_access(SwappableValues *this, void *po);

/* Get a whole array of swappables, i.e., the whole matrix.
 */
void swappable_values_accessm(SwappableValues **this, ulong n0, void *ppo);

/* Release a swappable object. If all holds of the object have been released,
 * it can be swapped out to storage if needed. The memory pointer is set back
 * to empty (null), and any references to it are no longer valid.
 */
void swappable_values_release(SwappableValues *this, void *po);

/* Release a whole array of swappables, i.e., the whole matrix.
 */
void swappable_values_releasem(SwappableValues **this, ulong n0, void *ppo);

/* Whether the swappable is unaccessed.
 */
bool swappable_values_is_unaccessed(SwappableValues *this);

/* Whether the swappable is in memory.
 */
bool swappable_values_is_in_memory(SwappableValues *this);

/* Whether the swappable is in storage.
 */
bool swappable_values_is_in_storage(SwappableValues *this);

/* Syntactic sugar for bytes.
 */
#define SwappableBytes SwappableValues

#define swappable_bytes_new(p_this, n) \
  swappable_values_new((p_this), byte, (n))

#define swappable_bytes_new_access(p_this, n, po) \
  swappable_values_new_access((p_this), byte, (n), (po))

#define swappable_bytes_newm(p_this, n0, n1) \
  swappable_values_newm((p_this), byte, (n0), (n1))

#define swappable_bytes_newm_access(p_this, n0, n1, ppo) \
  swappable_values_newm_access((p_this), byte, (n0), (n1), (ppo))

/* Syntactic sugar for ushorts.
 */
#define SwappableUshorts SwappableValues

#define swappable_ushorts_new(p_this, n) \
  swappable_values_new((p_this), ushort, (n))

#define swappable_ushorts_new_access(p_this, n, po) \
  swappable_values_new_access((p_this), ushort, (n), (po))

#define swappable_ushorts_newm(p_this, n0, n1) \
  swappable_values_newm((p_this), ushort, (n0), (n1))

#define swappable_ushorts_newm_access(p_this, n0, n1, ppo) \
  swappable_values_newm_access((p_this), ushort, (n0), (n1), (ppo))

/* Syntactic sugar for uints.
 */
#define SwappableUints SwappableValues

#define swappable_uints_new(p_this, n) \
  swappable_values_new((p_this), uint, (n))

#define swappable_uints_new_access(p_this, n, po) \
  swappable_values_new_access((p_this), uint, (n), (po))

#define swappable_uints_newm(p_this, n0, n1) \
  swappable_values_newm((p_this), uint, (n0), (n1))

#define swappable_uints_newm_access(p_this, n0, n1, ppo) \
  swappable_values_newm_access((p_this), uint, (n0), (n1), (ppo))

/* Syntactic sugar for ulongs.
 */
#define SwappableUlongs SwappableValues

#define swappable_ulongs_new(p_this, n) \
  swappable_values_new((p_this), ulong, (n))

#define swappable_ulongs_new_access(p_this, n, po) \
  swappable_values_new_access((p_this), ulong, (n), (po))

#define swappable_ulongs_newm(p_this, n0, n1) \
  swappable_values_newm((p_this), ulong, (n0), (n1))

#define swappable_ulongs_newm_access(p_this, n0, n1, ppo) \
  swappable_values_newm_access((p_this), ulong, (n0), (n1), (ppo))

/* Syntactic sugar for paws.
 */
#define SwappablePaws SwappableValues

#define swappable_paws_new(p_this, n) \
  swappable_values_new((p_this), paw, (n))

#define swappable_paws_new_access(p_this, n, po) \
  swappable_values_new_access((p_this), paw, (n), (po))

#define swappable_paws_newm(p_this, n0, n1) \
  swappable_values_newm((p_this), paw, (n0), (n1))

#define swappable_paws_newm_access(p_this, n0, n1, ppo) \
  swappable_values_newm_access((p_this), paw, (n0), (n1), (ppo))

/* Syntactic sugar for doubles.
 */
#define SwappableDoubles SwappableValues

#define swappable_doubles_new(p_this, n) \
  swappable_values_new((p_this), double, (n))

#define swappable_doubles_new_access(p_this, n, po) \
  swappable_values_new_access((p_this), double, (n), (po))

#define swappable_doubles_newm(p_this, n0, n1) \
  swappable_values_newm((p_this), double, (n0), (n1))

#define swappable_doubles_newm_access(p_this, n0, n1, ppo) \
  swappable_values_newm_access((p_this), double, (n0), (n1), (ppo))

/* Swappable objects.
 *
 * Constructor. This creates the swappable object but does not donate any
 * object to it yet.
 */
#define swappable_new(p_this, cls) \
  swappable_newI( \
      (p_this), \
      (void (*)(void **, struct File *))cls##_save, \
      (ulong (*)(void **))cls##_delete, \
      (ulong (*)(void **, struct File *))cls##_new_load);

void swappable_newI(
    Swappable **p_this,
    void saver(void **this, struct File *f),
    ulong destroy(void **this),
    ulong loader(void **this, struct File *f));

/* Same, for an array of swappables.
 */
#define swappables_new(p_this, n, cls) \
  swappables_newI( \
      (p_this), \
      (n), \
      (void (*)(void **, struct File *))cls##_save, \
      (ulong (*)(void **))cls##_delete, \
      (ulong (*)(void **, struct File *))cls##_new_load);

void swappables_newI(
    Swappable ***p_this,
    ulong n,
    void saver(void **this, struct File *f),
    ulong destroy(void **this),
    ulong loader(void **this, struct File *f));

/* Deposit constructor.
 */
#define swappable_new_deposit(p_this, cls, po) \
  swappable_new((p_this), cls); \
  swappable_deposit(*(p_this), (po));

/* Same, for an array of swappables.
 */
#define swappables_new_deposit(p_this, n, cls, ppo) \
  swappables_new((p_this), (n), cls); \
  swappables_deposit(*(p_this), (n), (ppo));

/* Destructor. If there is still an object deposited in the swappable that has
 * not been withdrawn, it is destroyed as well (but there must not be any
 * reacquisitions of the object that have not been released).
 */
void swappable_delete(Swappable **p_this);

/* Deposit an object in the swappable. The swappable takes ownership of the
 * object, saves it to a memory file, destroys the object, and releases that
 * memory file.
 */
void swappable_deposit(Swappable *this, void *po);

/* Same, for an array of swappables. The owning object pointers are taken over,
 * but the array is not.
 */
void swappables_deposit(Swappable **this, ulong n, void *ppo);

/* Withdraw the object deposited in the swappable back into its original
 * pointer, ending the assocation between the swappable and the object. The
 * swappable reacquires its memory file, loads the object back up from that
 * memory file into the original pointer, and destroys the memory file.
 */
void swappable_withdraw(Swappable *this);

/* Reacquire the object deposited in the swappable back into its original
 * pointer for potentially parallel access. (If more than one thread might
 * modify the reacquired object, suitable locks must be implemented by the
 * calling code; the swappable will not ensure this.) If the object has already
 * been reacquired, a reference count is simple incremented. Otherwise, the
 * swappable performs the same actions as for withdrawing the object, except
 * that it maintains its association with the object.
 */
void swappable_reacquire(Swappable *this);

/* Same, for an array of swappables.
 */
void swappables_reacquire(Swappable **a, ulong n);

/* Release a reacquisition of the object back into the swappable. If this is
 * the last reacquisition released, the swappable follows the same process as
 * for swappable_donate(). The pointer being released must be the one that the
 * object was reacquired into.
 */
void swappable_release(Swappable *this);

/* Same, for an array of swappables.
 */
void swappables_release(Swappable **a, ulong n);

/* Set the maximum number of bytes of swappable objects that can be in memory
 * at any time. If a program makes a request that makes this condition
 * impossible to satisfy, the request throws. Returns the previous limit.
 */
ulong swappable_set_in_memory_limit(ulong max_bytes);

/* Print function for the static class.
 */
void swappable_static_print(void *dummy, struct Printer *p);

/* String of the status of swappable memory overall. If no swappable memory has
 * been allocated, return without creating a string.
 */
void string_new_swappable_status(char **p_this, bool parenthesize);

/* Progress callback with a summary of the status of the Swappable factory.
 */
void swappable_progress(char **p_this, ulong count, ulong total_count, void *x);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
