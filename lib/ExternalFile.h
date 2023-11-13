/* Object class for files stored on a file system. This class, together with
 * MemoryFile, are wrapped in the overall File class.
 *
 * In write mode, the file is automatically written out to a "partial file"
 * filename, which is only renamed to the specified filename when the file has
 * been completely written out, flushed, and closed. This is similar to how
 * e.g. downloaded files are stored by web browsers. This prevents
 * multi-threaded or multi-process applications from acting on incomplete files
 * (and potentially failing because of that incompleteness). This class
 * automatically includes all appropriate logic for handling these partial
 * files, including when one is left over from a program execution that is
 * interrupted, throws an exception, or for any other reason fails to close the
 * file properly.
 *
 * This library also automatically and seamlessly handles Gzip compression of
 * files: any specified filename that ends in ".gz" is automatically read or
 * written as a Gzipped file, completely transparently to the calling code.
 */
#ifndef EXTERNAL_FILE_H
#define EXTERNAL_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bits.h"
#include <zlib.h>

typedef struct {
  boolbit append: 1;
  boolbit devnull: 1;
  boolbit read: 1;
  boolbit text: 1;
  boolbit update: 1;
  boolbit wrap: 1;
  boolbit write: 1;
  char *_filename;
  char *_mode;
  char *_partialFilename;
  FILE *ansifile;
  gzFile gzfile;
} ExternalFile;

#include "File.h"
#include "Printer.h"

/* Constructor. The 'mode' string is simply the ANSI mode, plus the following
 * shortcuts specific to this class:
 *
 *   filename    mode         result
 *   --------    ----         ------
 *     null     'stdin'   Read from stdin
 *     null    'stdout'   Write to stdout
 *     null    'stderr'   Write to stderr
 *      '-'       'r'     Read from stdin
 *      '-'       'w'     Write to stdout
 */
void external_file_new(ExternalFile **p_this, char *filename, char *mode);

/* Try constructor. The same special shortcuts as above are valid.
 */
bool external_file_try_new(ExternalFile **p_this, char *filename, char *mode);

/* Constructor that wraps an existing ANSI FILE. Does not close it when the
 * ExternalFile instance is destroyed. Useful for stdin, stdout, stderr (and
 * indeed is called from the above shortcuts).
 */
void external_file_new_wrap_ansifile(
    ExternalFile **p_this,
    FILE *ansifile,
    char *mode);

/* Destructor.
 */
void external_file_delete(ExternalFile **p_this);

/* Print.
 */
void external_file_print(ExternalFile *this, struct Printer *p);

/* Whether the file is gzipped.
 */
bool external_file_is_gzipped(ExternalFile *this);

/* Write a byte to the file.
 */
void external_file_write_byte(ExternalFile *this, byte b);

/* Write n bytes to the file.
 */
void external_file_write_bytes(ExternalFile *this, byte *bs, ulong n);

/* Write n bytes to a file with the specified filename.
 */
void external_file_write_bytes_to_filename(char *filename, byte *bs, ulong n);

/* Read a byte from the file.
 */
byte external_file_read_byte(ExternalFile *this);

/* Try to read a byte from the file.
 */
bool external_file_try_read_byte(ExternalFile *this, byte *b);

/* Read n bytes from a file.
 */
void external_file_read_bytes(ExternalFile *this, byte *bs, ulong n);

/* Read n bytes from a file, from its filename.
 */
void external_file_read_bytes_from_filename(char *filename, byte *bs, ulong n);

/* Get the size of the file. Not valid if the file is gzipped. Only makes sense
 * for modes in which the position can be saved and restored.
 */
ulong external_file_get_size(ExternalFile *this);

/* Get the current position in a binary-mode file. Not valid if the file is
 * gzipped or in text mode.
 */
ulong external_file_get_position(ExternalFile *this);

/* Set the position in a binary-mode file. Not valid if the file is gzipped or
 * in text mode.
 */
void external_file_set_position(ExternalFile *this, ulong position);

/* Rewind the file. Not valid if the file is gzipped.
 */
void external_file_rewind(ExternalFile *this);

/* Fast-forward to the end of the file. Not valid if the file is zipped.
 */
void external_file_fast_forward(ExternalFile *this);

/* Flush the file buffer. Valid for gzipped files as well.
 */
void external_file_flush(ExternalFile *this);

/* Whether the file's contents are equal to the specified blob of bytes.
 */
bool external_file_contents_equals_blob(
    ExternalFile *this,
    ulong len,
    byte *blob);

/* Assert that the file's contents equals the specified blob of bytes. Throws
 * an exception otherwise.
 */
void external_file_assert_contents_equals_blob(
    ExternalFile *this,
    ulong len,
    byte *blob);

/* Whether a file with this filename exists, in the sense that this program can
 * open this file for reading. If it exists on the filesystem but cannot be
 * opened for reading, say because of permissions, then it does not "exist" in
 * this sense. Note that if it doesn't "exist" in this sense, you can also test
 * below whether it is writable.
 */
bool external_file_exists(char *filename);

/* Whether a file with this filename exists, waiting up to max_seconds before
 * concluding that it doesn't.
 */
bool external_file_exists_wait(char *filename, byte max_seconds);

/* Whether a file with the partial filename for the given filename exists.
 */
bool external_file_partial_exists(char *filename);

/* Whether either a file with the given filename, or a file with its partial
 * version, or both, exists.
 */
bool external_file_either_exists(char *filename);

/* Whether the file with the given filename is empty. Throws if the file does
 * not exist.
 */
bool external_file_is_empty(char *filename);

/* Whether it is possible to write to the given filename, either as a new file
 * or appending to an existing file. Does not alter any existing file.
 */
bool external_file_is_writable(char *filename);

/* Create an empty file with the given filename. Throws a file with the given
 * filename already exists.
 */
void external_file_create(char *filename);

/* Remove the file with the given filename.
 */
void external_file_remove(char *filename);

/* Remove the file with the given filename if it exists.
 */
void external_file_remove_if_exists(char *filename);

/* Try to remove the file with the given filename.
 */
bool external_file_try_remove(char *filename);

/* Remove the file with the partial version of the given filename.
 */
void external_file_remove_partial(char *filename);

/* Remove both the file with the given filename and the file with the partial
 * version of it.
 */
void external_file_remove_both(char *filename);

/* Rename a file.
 */
void external_file_rename(char *orig_filename, char *new_filename);

/* Rename a file if it exists, but do not fail just because it does not.
 */
void external_file_rename_if_exists(char *orig_filename, char *new_filename);

/* Create a blob from the contents of an external file. Returns the length of
 * the blob.
 */
ulong blob_new_external_file_contents(byte **p_this, ExternalFile *ef);

/* Create a blob from the contents of an external file, from its filename.
 * Returns the length of the blob.
 */
ulong blob_new_external_file_contents_from_filename(
    byte **p_this,
    char *filename);

/* Create a string from the contents of an external file.
 */
void string_new_external_file_contents(char **p_this, ExternalFile *ef);

/* Create a string from the contents of an external file, from its filename.
 */
void string_new_external_file_contents_from_filename(
    char **p_this,
    char *filename);

/* Return the "progress status" of any file related to the given filename,
 * according to the logic of this class. Returns -1 if neither the file nor its
 * partial file exist; 0 if the partial file exists; +1 if the file exists.
 * These represent the three stages of processing (not started; processing;
 * completed).
 */
sbyte external_file_progress_status(char *filename);

/* Return the "progress status" of a set of files. Returns -1 if none of the
 * files or their partial files exist; +1 if all of the files exist; 0
 * otherwise.
 */
sbyte external_files_progress_status(ulong num_filenames, char **filenames);

/* Extract the pointer to the ANSI FILE within the given file object.
 */
FILE *external_file_get_ansifile(ExternalFile *this);

/* Whether the contents of the given ANSI FILE equals the specified blob of
 * bytes.
 */
bool ansifile_contents_equals_blob(FILE *file, ulong len, byte *blob);

/* Whether the contents of the ANSI FILE corresponding to the given filename
 * equals the specified blob of bytes. Useful for unit tests, as this function
 * does not use the ExternalFile class at all.
 */
bool ansifile_from_filename_contents_equals_blob(
    char *filename,
    ulong len,
    byte *blob);

/* Whether the contents of the given ANSI FILE equals the specified string.
 */
bool ansifile_contents_equals_string(FILE *file, char *s);

/* Whether the contents of the ANSI FILE corresponding to the given filename
 * equals the specified string. Again, useful for unit tests, as this function
 * does not use the ExternalFile class at all.
 */
bool ansifile_from_filename_contents_equals_string(char *filename, char *s);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
