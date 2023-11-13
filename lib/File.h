/* General object class for files, wrapping both MemoryFile and ExternalFile.
 */
#ifndef FILE_H
#define FILE_H

#ifdef __cplusplus
extern "C" {
#endif

struct File;

#include "ExternalFile.h"
#include "MemoryFile.h"
#include "Printer.h"

typedef struct File {
  struct MemoryFile *_mf;
  struct ExternalFile *_ef;
} File;

/* Constructor. The 'mode' string is simply the ANSI mode, plus the following
 * shortcuts specific to this class:
 *
 *   filename    mode         result
 *   --------    ----         ------
 *     null      null       MemoryFile
 *     null     'stdin'   Read from stdin
 *     null    'stdout'   Write to stdout
 *     null    'stderr'   Write to stderr
 *      '-'       'r'     Read from stdin
 *      '-'       'w'     Write to stdout
 */
void file_new(File **p_this, char *filename, char *mode);

/* Try constructor.
 */
bool file_try_new(File **p_this, char *filename, char *mode);

/* Constructor with the filename created from a format string.
 */
void file_new_f(File **p_this, char *mode, char *fmt, ...);

/* Try constructor with the filename created from a format string.
 */
bool file_try_new_f(File **p_this, char *mode, char *fmt, ...);

/* Constructor for a memory file.
 */
void file_new_memory(File **p_this);

/* Constructor for an external file.
 */
void file_new_external(File **p_this, char *filename, char *mode);

/* Try constructor for an external file.
 */
bool file_try_new_external(File **p_this, char *filename, char *mode);

/* Constructor, wrapping an ANSI FILE.
 */
void file_new_wrap_ansifile(File **p_this, FILE *ansifile, char *mode);

/* Constructor, copying into a memory file from an ExternalFile.
 */
void file_new_memory_copy_external(File **p_this, struct ExternalFile *ef);

/* Constructor, copying into a memory file from a filename.
 */
void file_new_memory_copy_from_filename(File **p_this, char *filename);

/* Constructor, copying in from a filename.
 */
void file_new_copy_from_filename(
    File **p_this,
    char *filename,
    char *mode,
    char *other_filename);

/* Constructor, copying in from another file.
 */
void file_new_copy(File **p_this, char *filename, char *mode, File *other);

/* Constructor, copying in from an ExternalFile.
 */
void file_new_copy_external(
    File **p_this,
    char *filename,
    char *mode,
    struct ExternalFile *ef);

/* Constructor for a memory file copied from a string.
 */
void file_new_memory_copy_string(File **p_this, char *s);

/* Destructor.
 */
void file_delete(File **p_this);

/* Print.
 */
void file_print(File *this, struct Printer *p);

/* Release the file. Only valid if it is a memory file.
 */
void file_release(File *this);

/* Reacquire the file. Only valid if it is a memory file.
 */
void file_reacquire(File *this);

/* Whether the file is gzipped.
 */
bool file_is_gzipped(File *this);

/* Whether the file is a memory file.
 */
bool file_is_memory(File *this);

/* Whether the file is an external file.
 */
bool file_is_external(File *this);

/* Get the filename of the file.
 */
char *file_get_filename(File *this);

/* Get the mode string for the file.
 */
char *file_get_mode(File *this);

/* Write a single byte to a file.
 */
void file_write_byte(File *this, byte b);

/* Write out num_bytes bytes to a file.
 */
void file_write_bytes(File *this, byte *bs, ulong num_bytes);

/* Write out a matrix of bytes to a file.
 */
void file_write_bytesm(File *this, byte **bm, ulong n0, ulong num_bytes);

/* Write out a value object in binary format.
 */
#define file_write_value(this, v) \
  file_write_bytes((this), (byte *)&(v), sizeof(v))

/* Write out an array of value objects in binary format.
 */
#define file_write_values(this, vs, n) \
  file_write_bytes((this), (byte *)(vs), (n) * sizeof(*(vs)))

/* Write out a matrix of value objects in binary format.
 */
#define file_write_valuesm(this, vm, n0, n1) \
  file_write_bytesm((this), (byte **)(vm), (n0), (n1) * sizeof(**(vm)))

/* Write a single character to a file.
 */
void file_write_char(File *this, char c);

/* Write a string to a file.
 */
void file_write_string(File *this, char *s);

/* Read a byte from a file.
 */
byte file_read_byte(File *this);

/* Try to read a byte from a file.
 */
bool file_try_read_byte(File *this, byte *b);

/* Read num_bytes bytes from a file.
 */
void file_read_bytes(File *this, byte *bs, ulong num_bytes);

/* Read a matrix of bytes from a file.
 */
void file_read_bytesm(File *this, byte **bm, ulong n0, ulong num_bytes);

/* Read a value object written out in binary format.
 */
#define file_read_value(this, pv) \
  file_read_bytes((this), (byte *)(pv), sizeof(*(pv)))

/* Read an array of value objects written out in binary format.
 */
#define file_read_values(this, vs, n) \
  file_read_bytes((this), (byte *)(vs), (n) * sizeof(*(vs)))

/* Read a matrix of value objects written out in binary format.
 */
#define file_read_valuesm(this, vm, n0, n1) \
  file_read_bytesm((this), (byte **)(vm), (n0), (n1) * sizeof(**(vm)))

/* Get the position in the file.
 */
ulong file_get_position(File *this);

/* Set the position of a file.
 */
void file_set_position(File *this, ulong position);

/* Rewind a file.
 */
void file_rewind(File *this);

/* Fast-forward a file to its end.
 */
void file_fast_forward(File *this);

/* Backup one byte in the file, i.e. decrement the position.
 */
void file_backup_one_byte(File *this);

/* Get the size of a file.
 */
ulong file_get_size(File *this);

/* Flush a file.
 */
void file_flush(File *this);

/* Whether the contents of a file equals a blob of bytes.
 */
bool file_contents_equals_blob(File *this, ulong len, byte *blob);

/* Assert that the contents of a file equals a blob of bytes.
 */
void file_assert_contents_equals_blob(File *this, ulong len, byte *blob);

/* Whether the contents of a file equals a string.
 */
bool file_contents_equals_string(File *this, char *s);

/* Assert that the contents of a file equals a string.
 */
void file_assert_contents_equals_string(File *this, char *s);

/* Copy the contents of another file into this one.
 */
void file_copy_other(File *this, File *other);

/* Copy the contents of an external file into this one.
 */
void file_copy_external(File *this, struct ExternalFile *ef);

/* Whether a file with this filename exists, in the sense that this program can
 * open this file for reading. If it exists on the filesystem but cannot be
 * opened for reading, say because of permissions, then it does not "exist" in
 * this sense. Note that if it doesn't "exist" in this sense, you can also test
 * below whether it is writable.
 */
bool file_exists(char *filename);

/* Whether a file with this filename exists, waiting up to max_seconds before
 * concluding that it doesn't.
 */
bool file_exists_wait(char *filename, byte max_seconds);

/* Whether a file with the partial filename for the given filename exists.
 */
bool file_partial_exists(char *filename);

/* Whether either a file with the given filename, or a file with its partial
 * version, or both, exists.
 */
bool file_either_exists(char *filename);

/* Whether the file with the given filename is empty. Throws if the file does
 * not exist.
 */
bool file_is_empty(char *filename);

/* Whether it is possible to write to the given filename, either as a new file
 * or appending to an existing file. Does not alter any existing file.
 */
bool file_is_writable(char *filename);

/* Create an empty file with the given filename. Throws a file with the given
 * filename already exists.
 */
void file_create(char *filename);

/* Remove the file with the given filename.
 */
void file_remove(char *filename);

/* Try to remove the file with the given filename.
 */
bool file_try_remove(char *filename);

/* Remove the file with the partial version of the given filename.
 */
void file_remove_partial(char *filename);

/* Remove both the file with the given filename and the file with the partial
 * version of it.
 */
void file_remove_both(char *filename);

/* Rename a file.
 */
void file_rename(char *orig_filename, char *new_filename);

/* Create a blob from the contents of a file. Returns the length of the bloc.
 */
ulong blob_new_file_contents(byte **p_this, File *f);

/* Create a blob from the contents of a file, from its filename. Returns the
 * length of the blob.
 */
ulong blob_new_file_contents_from_filename(byte **p_this, char *filename);

/* Create a string from the contents of a file.
 */
void string_new_file_contents(char **p_this, File *f);

/* Create a string from the contents of a file, from its filename.
 */
void string_new_file_contents_from_filename(char **p_this, char *filename);

/* Return the "progress status" of any file related to the given filename,
 * according to the logic of this class. Returns -1 if neither the file nor its
 * partial file exist; 0 if the partial file exists; +1 if the file exists.
 * These represent the three stages of processing (not started; processing;
 * completed).
 */
sbyte file_progress_status(char *filename);

/* Return the "progress status" of a set of files. Returns -1 if none of the
 * files or their partial files exist; +1 if all of the files exist; 0
 * otherwise.
 */
sbyte files_progress_status(ulong num_filenames, char **filenames);

/* Extract the pointer to the ANSI FILE within the given file object.
 */
FILE *file_get_ansifile(File *this);

/* Remove a file with the given filename.
 */
void file_remove(char *filename);

/* The following functions and syntactic sugar allow for objects to be
 * serialized out to a file in binary format in a generalized way.
 *
 * Save an object.
 */
#define save(o, c, f) c##_save((o), (f))

/* Save an object to an external file, specified by its filename.
 */
#define save_external(o, c, filename) { \
  File *_f___ = null; \
  \
  file_new(&_f___, (filename), "w"); \
  save((o), c, _f___); \
  file_delete(&_f___); \
}

/* Save an object to an external file, and verify that it is correct by
 * reloading and comparing to the original.
 */
#define save_external_verify(o, c, C, filename) { \
  C *_o___ = null; \
  \
  save_external((o), c, (filename)); \
  load_external(_o___, c, (filename)); \
  \
  if (!c##_equals(_o___, (o))) { \
    throw("Impossible: saved object '%s' corrupted", (filename)); \
  } \
  \
  c##_delete(&_o___); \
}

/* Load an object, which always constructs the object.
 */
#define load(o, c, f) c##_new_load(&(o), (f))

/* Load an object from an external file, specified by its filename.
 */
#define load_external(o, c, filename) { \
  File *_f___ = null; \
  \
  file_new(&_f___, (filename), "r"); \
  load((o), c, _f___); \
  file_delete(&_f___); \
}

/* Save an array of objects.
 */
#define save_array(os, n, c, f) { \
  ulong i___; \
  \
  for (i___ = 0; i___ < (n); i___++) { \
    save((os)[i___], c, (f)); \
  } \
}

/* Load an array of objects.
 */
#define load_array(os, n, c, f) { \
  ulong i___; \
  \
  for (i___ = 0; i___ < (n); i___++) { \
    load((os)[i___], c, (f)); \
  } \
}

/* Load an array of objects, creating the array first.
 */
#define load_new_array(os, n, c, f) \
  new(&(os), (n)); \
  load_array((os), (n), c, (f))

/* Save a matrix of objects.
 */
#define save_matrix(os, n0, n1, c, f) { \
  ulong j___; \
  \
  for (i___ = 0; i___ < (n0); i___++) { \
    save_array((os)[i___], (n1), c, (f)); \
  } \
}

/* Load a matrix of objects.
 */
#define load_matrix(os, n0, n1, c, f) { \
  ulong j___; \
  \
  for (i___ = 0; i___ < (n0); i___++) { \
    load_array((os)[i___], (n1), c, (f)); \
  } \
}

/* Load a matrix of objects, creating the matrix first.
 */
#define load_new_matrix(os, n0, n1, c, f) \
  newm(&(os), (n0), (n1)); \
  load_matrix((os), (n0), (n1), c, (f))

/* Save a ragged matrix of objects.
 */
#define save_rmatrix(os, n0, n1s, c, f) { \
  ulong j___; \
  \
  for (j___ = 0; j___ < (n0); j___++) { \
    save_array((os)[j___], (n1s)[j___], c, (f)); \
  } \
}

/* Load a ragged matrix of objects.
 */
#define load_rmatrix(os, n0, n1s, c, f) { \
  ulong j___; \
  \
  for (j___ = 0; j___ < (n0); j___++) { \
    load_array((os)[j___], (n1s)[j___], c, (f)); \
  } \
}

/* Load a ragged matrix of objects, creating the ragged matrix first.
 */
#define load_new_rmatrix(os, n0, n1s, c, f) \
  newrm(&(os), (n0), (n1s)); \
  load_rmatrix((os), (n0), (n1s), c, (f))

/* Save a value.
 */
#define save_value(v, f) file_write_value((f), (v))

/* Save an array of values.
 */
#define save_values(vs, n, f) file_write_values((f), (vs), (n))

/* Save a matrix of values.
 */
#define save_valuesm(vm, n0, n1, f) file_write_valuesm((f), (vm), (n0), (n1))

/* Load a value.
 */
#define load_value(v, f) file_read_value((f), &(v))

/* Load an array of values.
 */
#define load_values(vs, n, f) file_read_values((f), (vs), (n))

/* Load an array of values, creating the array first.
 */
#define load_new_values(vs, n, f) \
  new(&(vs), (n)), \
  file_read_values((f), (vs), (n)), \
  (n) * sizeof(*(vs))

/* Load a matrix of values.
 */
#define load_valuesm(vm, n0, n1, f) file_read_valuesm((f), (vm), (n0), (n1))

/* Save a label.
 */
#define save_label(s, f) save(s, string, f);

/* Load a label and verify that it is equal to the expected supplied string.
 */
void load_label(char *s, File *f);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
