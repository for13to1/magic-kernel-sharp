#include "Lock.h"

void lock(Lock *lock) {
  int err;

  if ((err = pthread_mutex_lock(lock)) != 0) {
    throw("lock() failed with error %d", err);
  }
}

void unlock(Lock *lock) {
  int err;

  if ((err = pthread_mutex_unlock(lock)) != 0) {
    throw("unlock() failed with error %d", err);
  }
}

void lock_initialize(Lock *lock) {
  int err;

  if ((err = pthread_mutex_init(lock, null)) != 0) {
    throw("lock_initialize() failed with error %d", err);
  }
}
