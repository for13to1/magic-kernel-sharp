#include "spawn.h"

void spawn_simple(char *command, byte retries) {
  int result;

  if ((result = try_spawn_simple(command)) != 0) {
    throw("Command '%s' returned error code %d", command, result);
  }
}

int try_spawn_simple(char *command) {
  return system(command);
}

void spawn(char *command,
    bool blocking,
    char **p_out_filename,
    char **p_err_filename,
    char **p_completed_filename) {
  int result;

  result = try_spawn(command,
      blocking,
      p_out_filename,
      p_err_filename,
      p_completed_filename);

  if (result != 0) {
    throw("Command '%s' returned error code %d", command, result);
  }
}

int try_spawn(char *command,
    bool blocking,
    char **p_out_filename,
    char **p_err_filename,
    char **p_completed_filename) {
  char *_full_command = null;
  int result;
  TempFilename *_out_filename = null;
  TempFilename *_err_filename = null;
  TempFilename *_completed_filename = null;

  temp_filename_new_keep(&_out_filename, "txt", p_out_filename != null);
  temp_filename_new_keep(&_err_filename, "txt", p_err_filename != null);
  temp_filename_new_keep(
      &_completed_filename,
      "txt",
      p_completed_filename != null);

  string_new_f(
      &_full_command,
      "exec 1>%s; exec 2>%s; %s%s && touch %s%s",
      _out_filename->_filename,
      _err_filename->_filename,
      blocking ? "" : "nohup ",
      command,
      _completed_filename->_filename,
      blocking ? "" : " &");

  result = system(_full_command);

  string_delete(&_full_command);

  if (p_out_filename != null) {
    donate(&_out_filename->_filename, p_out_filename);
  }

  if (p_err_filename != null) {
    donate(&_err_filename->_filename, p_err_filename);
  }

  if (p_completed_filename != null) {
    donate(&_completed_filename->_filename, p_completed_filename);
  }

  temp_filename_delete(&_completed_filename);
  temp_filename_delete(&_err_filename);
  temp_filename_delete(&_out_filename);

  return result;
}
