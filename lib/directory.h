/* Static library for file system directory utilities.
 */
#ifndef DIRECTORY_H
#define DIRECTORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "qsortx.h"
#include "File.h"

/* Check if a directory exists.
 */
bool directory_exists(char *dirname);

/* Check if a directory is empty, i.e. exists but contains no files or other
 * directories.
 */
bool directory_is_empty(char *dirname);

/* Read the entries of a directory. The number of entries is returned, and the
 * array of sorted strings of entries is stored in *p_entries. If starts_with
 * and/or ends_with are not null, only the entries matching these constraints
 * are returned.
 */
ulong directory_read(
    char *dirname,
    char *starts_with,
    char *ends_with,
    bool ignore_case,
    bool include_dirname,
    char ***p_entries);

/* Try to read the entries of a directory.
 */
bool directory_try_read(
    char *dirname,
    char *starts_with,
    char *ends_with,
    bool ignore_case,
    bool include_dirname,
    ulong *num_entries,
    char ***p_entries);

/* Ensure that a directory exists. If it doesn't, create it.
 */
void directory_ensure_exists(char *dirname);

/* Ensure that the directory part of a path exists. If it doesn't, create it.
 */
void directory_ensure_exists_for_path(char *path);

/* Remove a directory.
 */
void directory_remove(char *dirname);

/* Rename a directory, which actually checks that each directory in the path is
 * renamed where necessary.
 */
void directory_rename(char *old_dirname, char *new_dirname);

/* Rename a path, which renames whichever parts of the directory and filename
 * are necessary to effect the rename.
 */
void path_rename(char *old_path, char *new_path);

/* Remove a path, which removes the file, and then prunes its directory.
 */
void path_remove(char *path);

/* Prune the directory of a path.
 */
void path_prune_directory(char *path);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
