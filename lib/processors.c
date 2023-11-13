#include "processors.h"

static ulong total_processors_ = 0;
static Lock lock_ = LOCK_INITIALIZER;


ulong processors_get_total(void) {
  char *_out = null;
  char *_err = null;
  char *_completed = null;
  ulong n;
  TextFile *_tf = null;

  if (total_processors_ > 0) {
    return total_processors_;
  }

  lock(&lock_);

  if (total_processors_ > 0) {
    return total_processors_;
  }

  /* Works on Linux.
   */
  if (file_exists("/proc/cpuinfo")) {
    spawn("cat /proc/cpuinfo | grep processor | wc -l",
        true,
        &_out,
        &_err,
        &_completed);

    if (external_file_is_empty(_err)) {
      text_file_new(&_tf, _out, "r");
      n = text_file_read_single_ulong(_tf);
      text_file_delete(&_tf);

      external_file_try_remove(_completed);
      external_file_try_remove(_out);
      external_file_try_remove(_err);

      string_delete(&_completed);
      string_delete(&_out);
      string_delete(&_err);

      total_processors_ = n;

      unlock(&lock_);

      return n;
    }
  }

  /* Linux command failed. Clean up.
   */
  external_file_try_remove(_completed);
  external_file_try_remove(_out);
  external_file_try_remove(_err);

  string_delete(&_completed);
  string_delete(&_out);
  string_delete(&_err);

  /* This works on macOS.
   */
  spawn("sysctl -n hw.ncpu", true, &_out, &_err, &_completed);

  if (external_file_is_empty(_err)) {
    text_file_new(&_tf, _out, "r");
    n = text_file_read_single_ulong(_tf);
    text_file_delete(&_tf);

    external_file_try_remove(_completed);
    external_file_try_remove(_out);
    external_file_try_remove(_err);

    string_delete(&_completed);
    string_delete(&_out);
    string_delete(&_err);

    total_processors_ = n;

    unlock(&lock_);

    return n;
  }

  unlock(&lock_);

  throw("Error in catting /proc/cpuinfo: see '%s'", _err);

  return 0;
}

ulong processors_get_free(void) {
  char *_out = null;
  char *_err = null;
  char *_completed = null;
  char *field;
  char *_e = null;
  ulong n;
  ulong index;
  ulong total;
  double idle;
  TextFile *_tf = null;

  total = processors_get_total();

  if (try_spawn("mpstat 1 1", true, &_out, &_err, &_completed) != 0) {
    external_file_try_remove(_completed);
    external_file_try_remove(_out);
    external_file_try_remove(_err);

    string_delete(&_completed);
    string_delete(&_out);
    string_delete(&_err);

    return processors_get_total();
  }

  if (!external_file_is_empty(_err)) {
    throw("Error in mpstat: see '%s'", _err);
  }

  text_file_new(&_tf, _out, "r");

  /* Check that we understand the output.
   */
  string_new_f(&_e, "(%lu CPU)", total);
  field = text_file_read_single(_tf);

  if (!string_ends_with(field, _e, false)) {
    throw("First line '%s' of mpstat did not end with '%s'", field, _e);
  }

  string_delete(&_e);

  field = text_file_read_single(_tf);

  if (!string_equals(field, "")) {
    throw("Second line '%s' of mpstat was not empty", field);
  }

  field = text_file_read_single(_tf);

  if (!string_ends_with(field, "   %idle", false)) {
    throw("Third line '%s' of mpstat did not end with '  %%idle'", field);
  }

  field = text_file_read_single(_tf);

  if (!string_contains_at(field, 13, "all")) {
    throw("Fourth line '%s' of mpstat did not contain 'all' at position 13",
        field);
  }

  index = string_last_index_of(field, ' ');
  idle = parse_double(field + index + 1);

  n = (ulong)round(idle * 0.01 * total);

  text_file_delete(&_tf);

  external_file_try_remove(_completed);
  external_file_try_remove(_out);
  external_file_try_remove(_err);

  string_delete(&_completed);
  string_delete(&_out);
  string_delete(&_err);

  return n;
}

ulong processors_get_memory_per(void) {
  return memory_get_total() / processors_get_total();
}
