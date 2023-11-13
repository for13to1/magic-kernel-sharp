#include "LockTest.h"

START

TEST(Lock, Basic) {
  Lock lock_ = LOCK_INITIALIZER;

  lock(&lock_);
  unlock(&lock_);
}

FINISH
