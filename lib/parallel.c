#include "parallel.h"
#include <limits.h>
#include <pthread.h>
#include <unistd.h>

#define MIN_ 100000

typedef struct {
  Parallel par;
  void (*run)(Parallel *par);
} Pass_;

static ulong max_threads_ = 0;
static ulong running_ = 0;
static Lock lock_ = LOCK_INITIALIZER;
static ulong denominator_ = 0;

static void *run_(void *x);

static ulong available_(void);


void parallel_for(ulong n, ulong min, void (*run)(Parallel *par), void *x) {
  parallel_for_specify(n, parallel_num_threads(n, min), run, x);
}

void parallel_for_specify(
    ulong n,
    ulong num_threads,
    void (*run)(Parallel *par),
    void *x) {
  bool fail = false;
  bool *_started = null;
  bool *_finished = null;
  ulong num_waiting;
  ulong start;
  ulong t;
  void *status;
  Parallel single_par;
  Parallel *par;
  Pass_ *pass;
  Pass_ *_passes = null;
  pthread_t *_threads = null;
  Random *_random = null;

  if (num_threads == 0) {
    throw("Cannot specify no threads");
  }

  if (num_threads == 1) {
    single_par.numThreads = 1;
    single_par.t = 0;
    single_par.start = 0;
    single_par.end = n;
    single_par.x = x;

    run(&single_par);

    return;
  }

  new(&_passes, num_threads);

  for (t = 0, start = 0; t < num_threads; t++) {
    pass = &_passes[t];
    pass->run = run;

    par = &pass->par;
    par->numThreads = num_threads;
    par->t = t;
    par->start = start;
    par->end = start = (t + 1) * n / num_threads;
    par->x = x;
  }

  new(&_threads, num_threads);
  new(&_started, num_threads);
  new(&_finished, num_threads);

  if (denominator_ != 0) {
    random_new(&_random);
  }

  do {
    for (t = 0, num_waiting = 0; t < num_threads; t++) {
      if (!_finished[t]) {
        if (denominator_ != 0) {
          fail = random_modulo(_random, denominator_) == 0;
        }

        if (fail
            || pthread_create(&_threads[t], null, run_, &_passes[t]) != 0) {
          num_waiting++;
        } else {
          _started[t] = true;
        }
      }
    }

    for (t = 0; t < num_threads; t++) {
      if (_started[t] && !_finished[t]) {
        if (pthread_join(_threads[t], &status) != 0) {
          throw("Impossible: Thread #%lu did not successfully complete", t);
        } else {
          _finished[t] = true;
        }

        if (status != null) {
          throw("Parallel function for thread %lu returned non-null", t);
        }
      }
    }

    if (num_waiting > 0) {
      sleep(1);
    }
  } while (num_waiting > 0);

  for (t = 0; t < num_threads; t++) {
    if (!_started[t]) {
      throw("Impossible: thread %lu not started", t);
    }

    if (!_finished[t]) {
      throw("Impossible: thread %lu not finished", t);
    }
  }

  if (denominator_ != 0) {
    random_delete(&_random);
  }

  deletev(&_finished);
  deletev(&_started);
  deletev(&_threads);
  deletev(&_passes);
}

void parallel_do(void (*run)(Parallel *par),void *x) {
  parallel_for_specify(0, available_(), run, x);
}

ulong parallel_num_threads(ulong n, ulong min) {
  ulong available;
  ulong num_threads;
  ulong total_processors;

  if (n == 0) {
    return 1;
  }

  if (max_threads_ == 0) {
    lock(&lock_);

    if (max_threads_ == 0) {
      total_processors = processors_get_total();
      max_threads_ = maximum(total_processors - 1, 1);
    }

    unlock(&lock_);
  }

  available = available_();

  if (available == 1 || n < min) {
    num_threads = 1;
  } else if (min < 2) {
    num_threads = minimum(n, available);
  } else {
    num_threads = minimum(1 + (n - 1) / min, available);
  }

  if (num_threads == 0) {
    throw("Impossible");
  }

  return num_threads;
}

void parallel_override_max_num_threads(ulong num_threads) {
  lock(&lock_);
  max_threads_ = num_threads;
  unlock(&lock_);
}

void parallel_simulate_resource_constraints(ulong denominator) {
  denominator_ = denominator;
}


static void *run_(void *x) {
  Pass_ *pass;

  lock(&lock_);
  running_++;
  unlock(&lock_);

  pass = x;

  pass->run(&pass->par);

  lock(&lock_);
  running_--;
  unlock(&lock_);

  return null;
}

static ulong available_(void) {
  ulong available;

  lock(&lock_);
  available = maximum(max_threads_ - minimum(running_, max_threads_), 1);
  unlock(&lock_);

  return available;
}
