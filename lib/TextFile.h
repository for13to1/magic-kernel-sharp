/* Object class for reading and writing records and fields for character-
 * separated values text files (CSV, TSV, etc.) reasonably efficiently.
 *
 * The 'mode' argument is the standard ANSI FILE mode string.
 *
 * Unix, DOS, and old Mac formats are all handled transparently for reading.
 * Unix format is used for writing.
 *
 * As this class uses the File class, Gzip compression is automatically
 * handled, simply by ending the filename with ".gz".
 *
 * To simplify processing and avoid "escape hell," this class does not provide
 * any escape handling. Rather, the separator character is assumed to have been
 * selected so as to make escaping unnecessary.
 *
 * This leads to a natural hierarchy of file types:
 *
 *   CSV: Comma-separated. Cannot be used if any field contains commas.
 *   TSV: Tab-separated. Handles fields with commas, but not those with tabs.
 *   HSV: Hive-separated (i.e. \001-separated). Handles commas and tabs.
 *   NSV: Null-separated (i.e. \0-separated). Handles all valid UTF-8 strings.
 *   TXT: Equivalent to NSV, but generally used to denote single-field records.
 */
#ifndef TEXT_FILE_H
#define TEXT_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

struct paw;

#include "File.h"
#include "numbers.h"
#include "paw.h"

typedef struct {
  bool justReadCR;
  char separator;
  char *_record;
  char **_fields;
  ulong maxNumRecordChars;
  ulong maxNumFields;
  ulong numFields;
  ulong currentFieldIndex;
  ulong numRecordChars;
  struct File *_file;
} TextFile;

/* Constructor, inferring the type from the filename extension.
 */
void text_file_new(TextFile **p_this, char *filename, char *mode);

/* Constructor, specifying the type as "csv", "tsv", "hsv", "nsv", or "txt".
 * The mode shortcuts listed in File.h can be used with this constructor.
 */
void text_file_new_type(
    TextFile **p_this,
    char *filename,
    char *mode,
    char *filetype);

/* Constructor for a memory file.
 */
void text_file_new_memory(TextFile **p_this, char *filetype);

/* Try constructor, inferring the type from the filename.
 */
bool text_file_try_new(TextFile **p_this, char *filename, char *mode);

/* Try constructor, specifying the type.
 */
bool text_file_try_new_type(
    TextFile **p_this,
    char *filename,
    char *mode,
    char *filetype);

/* Destructor.
 */
void text_file_delete(TextFile **p_this);

/* Print.
 */
void text_file_print(TextFile *this, struct Printer *p);

/* Whether a filetype is valid.
 */
bool text_file_type_is_valid(char *filetype);

/* Write a field.
 */
void text_file_write(TextFile *this, char *s);

/* Write a formatted field.
 */
void text_file_write_f(TextFile *this, char *fmt, ...);

/* Write a field from a bool, as the corresponding bool string ("true" or
 * "false").
 */
void text_file_write_bool(TextFile *this, bool b);

/* Write a field from a ulong.
 */
void text_file_write_ulong(TextFile *this, ulong u);

/* Write a field as a serialized ulong.
 */
void text_file_write_serialized_ulong(TextFile *this, ulong u);

/* Write a field from a long.
 */
void text_file_write_long(TextFile *this, long l);

/* Write a field from a paw.
 */
void text_file_write_paw(TextFile *this, struct paw p);

/* Write a field from a double.
 */
void text_file_write_double(TextFile *this, double d);

/* Write a field as a serialized double.
 */
void text_file_write_serialized_double(TextFile *this, double d);

/* Write an empty field.
 */
void text_file_write_empty_field(TextFile *this);

/* End the record, writing out a newline.
 */
void text_file_end_record(TextFile *this);

/* Write out a complete record.
 */
void text_file_write_record(TextFile *this, ulong num_fields, char **fields);

/* Write out a complete record that consists of doubles.
 */
void text_file_write_doubles_record(
    TextFile *this,
    ulong num_fields,
    double *fields);

/* Write an empty record. For parity with read methods.
 */
void text_file_write_empty_record(TextFile *this);

/* Write a single-field record.
 */
void text_file_write_single(TextFile *this, char *s);

/* Write a single-field ulong record.
 */
void text_file_write_single_ulong(TextFile *this, ulong u);

/* Write a single-field record as a serialized ulong.
 */
void text_file_write_single_serialized_ulong(TextFile *this, ulong u);

/* Write a single-field long record.
 */
void text_file_write_single_long(TextFile *this, long l);

/* Write a single-field record as a serialized double.
 */
void text_file_write_single_serialized_double(TextFile *this, double d);

/* Write a single-field double record.
 */
void text_file_write_single_double(TextFile *this, double d);

/* Write a label record, i.e. a record with one field containing a label. For
 * parity with read methods.
 */
void text_file_write_label(TextFile *this, char *label);

/* Write a labeled record, i.e. a record with two fields, the first being a
 * label.
 */
void text_file_write_labeled(TextFile *this, char *label, char *s);

/* Write a labeled bool record.
 */
void text_file_write_labeled_bool(TextFile *this, char *label, bool b);

/* Write a labeled byte record.
 */
void text_file_write_labeled_byte(TextFile *this, char *label, byte b);

/* Write a labeled ulong record.
 */
void text_file_write_labeled_ulong(TextFile *this, char *label, ulong u);

/* Read a record. Returns the number of fields. The 'fields' pointer is *not* a
 * new array; it is only a reference to an array inside the TextFile. To retain
 * any field through the next read operation, copy it. To copy all fields, use
 * text_file_read_copy() below.
 */
ulong text_file_read(TextFile *this, char ***fields);

/* Try to read a record. Returns whether successful.
 */
bool text_file_try_read(TextFile *this, ulong *num_fields, char ***fields);

/* Read a record, where the fields pointer is a new copy of the fields.
 */
void text_file_read_copy(TextFile *this, ulong *num_fields, char ***p_fields);

/* Try to read a record, where the fields pointer is a new copy of the fields
 * if successful.
 */
bool text_file_try_read_copy(
    TextFile *this,
    ulong *num_fields,
    char ***p_fields);

/* Read and discard a record.
 */
void text_file_read_discard(TextFile *this);

/* Try to read a record, discarding it if we do.
 */
bool text_file_try_read_discard(TextFile *this);

/* Read an empty record. Throws if it was not empty.
 */
void text_file_read_empty_record(TextFile *this);

/* Read a single-field record. Throw if there is more than one field. An empty
 * record is considered to have one empty-string field, as these are
 * indistinguishable for text files.
 */
char *text_file_read_single(TextFile *this);

/* Try to read a single-field record.
 */
bool text_file_try_read_single(TextFile *this, char **s);

/* Read a ulong single-field record.
 */
ulong text_file_read_single_ulong(TextFile *this);

/* Read a serialized double single-field record.
 */
double text_file_read_single_serialized_double(TextFile *this);

/* Read a label record, i.e., read a single-field label record and confirm that
 * it is the expected string.
 */
void text_file_read_label(TextFile *this, char *label);

/* Read a labeled field, i.e. a two-field record, confirming that the first
 * field equals the label, and returning a pointer to the string of the second
 * field..
 */
char *text_file_read_labeled(TextFile *this, char *label);

/* Read a labeled bool record, returning the bool value.
 */
bool text_file_read_labeled_bool(TextFile *this, char *label);

/* Read a labeled byte record, returning the byte value.
 */
byte text_file_read_labeled_byte(TextFile *this, char *label);

/* Read a labeled ulong record, returning the byte value.
 */
ulong text_file_read_labeled_ulong(TextFile *this, char *label);

/* Rewind the TextFile.
 */
void text_file_rewind(TextFile *this);

/* The filename of the text file, or null if it is a memory file.
 */
char *text_file_get_filename(TextFile *this);

/* Create a string from the contents of a TextFile.
 */
void string_new_text_file_contents(char **p_this, TextFile *tf);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
