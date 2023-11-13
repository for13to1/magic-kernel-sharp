#include "memory.h"

static ulong total_memory_ = 0;
static Lock lock_ = LOCK_INITIALIZER;


ulong memory_get_total(void) {
  char *line;
  char *_completed = null;
  char *_err = null;
  char *_out = null;
  char *_sub = null;
  long l;
  TextFile *_tf = null;

  /* If this is initialized, just return the value.
   */
  if (total_memory_ != 0) {
    return total_memory_;
  }

  /* Otherwise, lock to initialize.
   */
  lock(&lock_);

  /* Check if it was initialized by another thread while we were locked.
   */
  if (total_memory_ != 0) {
    unlock(&lock_);

    return total_memory_;
  }

  /* This works on Linux.
   */
  if (file_exists("/proc/meminfo")) {
    text_file_new_type(&_tf, "/proc/meminfo", "r", "txt");
    line = text_file_read_single(_tf);

    if (!string_ends_with(line, " kB", false)) {
      throw("Expected first line '%s' of /proc/meminfo to end in ' kB'", line);
    }

    l = string_nth_last_index_of(line, 1, ' ');

    if (l < 0) {
      throw("Couldn't find two spaces in '%s'", line);
    }

    string_new_substring(&_sub, line, l + 1, string_length(line) - (l + 1) - 3);
    total_memory_ = parse_ulong(_sub) * 1024;
    string_delete(&_sub);
    text_file_delete(&_tf);
  } else {
    /* This works on macOS.
     */
    spawn("sysctl -n hw.memsize", true, &_out, &_err, &_completed);

    if (external_file_is_empty(_err)) {
      text_file_new(&_tf, _out, "r");
      total_memory_ = text_file_read_single_ulong(_tf);
      text_file_delete(&_tf);

      external_file_try_remove(_completed);
      external_file_try_remove(_out);
      external_file_try_remove(_err);

      string_delete(&_completed);
      string_delete(&_out);
      string_delete(&_err);
    }
  }

  unlock(&lock_);

  if (total_memory_ == 0) {
    throw("Could not determine total memory");
  }

  return total_memory_;
}
