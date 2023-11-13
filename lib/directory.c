#include "directory.h"
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

static void ensure_(char *dirname);


bool directory_exists(char *dirname) {
  DIR *dir;

  if ((dir = opendir(dirname)) == null) {
    return false;
  }

  closedir(dir);

  return true;
}

bool directory_is_empty(char *dirname) {
  char **_entries = null;
  ulong num_entries;

  if (!directory_exists(dirname)) {
    throw("Directory '%s' does not exist", dirname);
  }

  num_entries = directory_read(dirname, null, null, false, false, &_entries);
  delete(&_entries, num_entries, string_delete);

  return num_entries == 0;
}

ulong directory_read(
    char *dirname,
    char *starts_with,
    char *ends_with,
    bool ignore_case,
    bool include_dirname,
    char ***p_entries) {
  ulong num_entries;

  if (!directory_try_read(
        dirname,
        starts_with,
        ends_with,
        ignore_case,
        include_dirname,
        &num_entries,
        p_entries)) {
    throw("Could not read the directory '%s'", dirname);
  }

  return num_entries;
}

bool directory_try_read(
    char *dirname,
    char *starts_with,
    char *ends_with,
    bool ignore_case,
    bool include_dirname,
    ulong *num_entries,
    char ***p_entries) {
  char *name;
  char *_sanitized = null;
  ulong n;
  DIR *dir;
  struct dirent *dp;

  if ((dir = opendir(dirname)) == null) {
    return false;
  }

  n = 0;
  new(p_entries, 0);

  if (include_dirname) {
    string_new_copy(&_sanitized, dirname);
    string_renew_ensure_ends_with(&_sanitized, "/", false);
  }

  while ((dp = readdir(dir)) != null) {
    name = dp->d_name;

    if (string_equals(name, ".") || string_equals(name, "..")) {
      continue;
    }

    if (starts_with != null
        && !string_starts_with(name, starts_with, ignore_case)) {
      continue;
    }

    if (ends_with != null
        && !string_ends_with(name, ends_with, ignore_case)) {
      continue;
    }

    renewup1(p_entries, n);

    if (include_dirname) {
      string_new_concat_until_null(
          &(*p_entries)[n],
          _sanitized,
          name,
          null);
    } else {
      string_new_copy(&(*p_entries)[n], name);
    }

    n++;
  }

  string_delete(&_sanitized);

  closedir(dir);

  *num_entries = n;

  qsortx(*p_entries, n, string_pointer_compare, false, null);

  return true;
}

void directory_ensure_exists(char *dirname) {
  char *_partial = null;
  char **_parts = null;
  ulong n;
  ulong u;

  strings_new_split(&_parts, &n, dirname, '/');

  string_new_empty(&_partial);

  for (u = 0; u < n; u++) {
    string_renew_append_until_null(&_partial, _parts[u], "/", null);
    ensure_(_partial);
  }

  string_delete(&_partial);
  delete(&_parts, n, string_delete);
}

void directory_ensure_exists_for_path(char *path) {
  char *_dirname = null;

  string_new_directory_from_path(&_dirname, path);
  directory_ensure_exists(_dirname);
  string_delete(&_dirname);
}

void directory_remove(char *dirname) {
  file_remove(dirname);
}

void directory_prune(char *dirname) {
  char **_parts = null;
  char **_partials = null;
  ulong n;
  ulong u;
  long l;

  strings_new_split(&_parts, &n, dirname, '/');
  new(&_partials, n);

  for (u = 0; u < n; u++) {
    if (u == 0) {
      string_new_concat_until_null(&_partials[0], _parts[0], "/", null);
    } else {
      string_new_concat_until_null(
          &_partials[u],
          _partials[u - 1],
          _parts[u],
          "/",
          null);
    }
  }

  delete(&_parts, n, string_delete);

  for (l = (long)n - 1; l >= 0; l--) {
    if (directory_is_empty(_partials[l])) {
      directory_remove(_partials[l]);
    } else {
      break;
    }
  }

  delete(&_partials, n, string_delete);
}

void directory_rename(char *old_dirname, char *new_dirname) {
  char *_new_partial = null;
  char *_old_partial = null;
  char **_new_parts = null;
  char **_old_parts = null;
  ulong n_new;
  ulong n_old;
  ulong u;

  strings_new_split(&_old_parts, &n_old, old_dirname, '/');
  strings_new_split(&_new_parts, &n_new, new_dirname, '/');

  if (n_new != n_old) {
    throw("Cannot rename directory '%s' with %lu part%s to '%s' "
        "with %lu part%s",
        old_dirname,
        n_old,
        plural_s(n_old),
        new_dirname,
        n_new,
        plural_s(n_new));
  }

  string_new_empty(&_old_partial);
  string_new_empty(&_new_partial);

  for (u = 0; u < n_old; u++) {
    string_renew_append_until_null(&_old_partial, _old_parts[u], "/", null);
    string_renew_append_until_null(&_new_partial, _new_parts[u], "/", null);

    if (!string_equals(_new_partial, _old_partial)) {
      if (directory_exists(_new_partial)) {
        throw("Directory '%s' already exists, so cannot rename '%s' to it",
            _new_partial,
            _old_partial);
      }

      file_rename(_old_partial, _new_partial);
      string_delete(&_old_partial);
      string_new_copy(&_old_partial, _new_partial);
    }
  }

  string_delete(&_new_partial);
  string_delete(&_old_partial);

  delete(&_new_parts, n_old, string_delete);
  delete(&_old_parts, n_old, string_delete);
}

void path_rename(char *old_path, char *new_path) {
  char *_new_directory = null;
  char *_new_filename = null;
  char *_old_directory = null;
  char *_old_filename = null;
  char *_old_path = null;

  strings_new_directory_filename_from_path(
      &_old_directory,
      &_old_filename,
      old_path);
  strings_new_directory_filename_from_path(
      &_new_directory,
      &_new_filename,
      new_path);

  if (!string_equals(_old_directory, _new_directory)) {
    directory_rename(_old_directory, _new_directory);
  }

  if (!string_equals(_old_filename, _new_filename)) {
    string_new_concat_until_null(
        &_old_path,
        _new_directory,
        "/",
        _old_filename,
        null);
    file_rename(_old_path, new_path);
    string_delete(&_old_path);
  }

  string_delete(&_new_filename);
  string_delete(&_new_directory);
  string_delete(&_old_filename);
  string_delete(&_old_directory);
}

void path_remove(char *path) {
  file_remove(path);
  path_prune_directory(path);
}

void path_prune_directory(char *path) {
  char *_dirname = null;

  string_new_directory_from_path(&_dirname, path);
  directory_prune(_dirname);
  string_delete(&_dirname);
}


static void ensure_(char *dirname) {
  int status;

  if (directory_exists(dirname)) {
    return;
  }

  status = mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  if (status != 0 && errno != EEXIST) {
    throw("Could not ensure that directory '%s' exists: %s",
        dirname,
        strerror(errno));
  }
}
