/* Strings. See description in jpc.h. An immutable-size class.
 */
#ifndef STRNG_H
#define STRNG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jpc.h"
#include <stdarg.h>

/* Create a new empty string, correctly '\0'-terminated.
 */
void string_new_empty(char **p_this);

/* Create a new string of length 1 that is a copy of a char.
 */
void string_new_char(char **p_this, char c);

/* Copy constructor.
 */
void string_new_copy(char **p_this, char *other);

/* Load.
 */
void string_new_load(char **p_this, struct File *f);

/* Load when the length has already been loaded. Used for the above and also
 * for the verify() macro.
 */
void string_new_load_from_length(char **p_this, ulong length, struct File *f);

/* Create a new string that is a lowercase copy of another.
 */
void string_new_copy_lowercase(char **p_this, char *other);

/* Create a new string from a printf-like format specification.
 */
void string_new_f(char **p_this, char *fmt, ...);

/* Create a new string from a printf-like format specification as va_list.
 */
void string_new_vf(char **p_this, char *fmt, va_list ap1, va_list ap2);

/* Create a new string by concatenating two strings.
 */
void string_new_concat(char **p_this, char *first, char *second);

/* Create a new string by concatenating any number of strings. Each vararg must
 * be of type char *. The last argument must be a null sentinel.
 */
void string_new_concat_until_null(char **p_this, ...);

/* Create a new string by concatenating an array of strings.
 */
void string_new_concat_array(char **p_this, ulong n, char **others);

/* Create a string by truncating another string to len characters. Fails with
 * error if len is greater than the length of the string.
 */
void string_new_truncate(char **p_this, char *other, ulong len);

/* Create a string by shortening another string by n characters. Fails with
 * error if n is greater than the length of the string.
 */
void string_new_shorten(char **p_this, char *other, ulong n);

/* Create a string by indenting another string by adding n lots of a standard
 * indent string (by default, ". ") at the start of every line.
 */
void string_new_indent(char **p_this, char *other, ulong n);

/* Extracts the filename from a path.
 */
void string_new_filename_from_path(char **p_this, char *path);

/* Same, but replacing the existing.
 */
void string_renew_filename_from_path(char **p_this, char *path);

/* Extracts the directory from a path, and standardizes it.
 */
void string_new_directory_from_path(char **p_this, char *path);

/* Standardize a string containing a directory by removing multiple consecutive
 * slashes and any trailing slash.
 */
void string_new_standardize_directory(char **p_this, char *directory);

/* Same, in-place.
 */
void string_renew_standardize_directory(char **p_this);

/* Extract the directory and filename from a path.
 */
void strings_new_directory_filename_from_path(
    char **directory,
    char **filename,
    char *path);

/* Extract the extension from a path, after first stripping off any ".gz", and
 * converting to lowercase. If no extension is found (after removing any
 * ".gz"), the string pointer remains null.
 */
void string_new_extension_from_path(char **p_this, char *path);

/* Extract whether the path is gzipped (ends in ".gz") and the extension
 * (stripping off any ".gz" if present), converting to lowercase. If no
 * extension is found (after removing any ".gz"), the string pointer remains
 * null.
 */
void string_new_extension_gzipped_from_path(
    char **p_this,
    bool *gzipped,
    char *path);

/* Create a string by stripping off any extension from a path, after removing
 * any ".gz" if present.
 */
void string_new_remove_extension(char **p_this, char *path);

/* Create a new string, trimming any whitespace from the start and end of the
 * other string.
 */
void string_new_trim_whitespace(char **p_this, char *other);

/* Trim any whitespace from the start and end of a string, in-place.
 */
void string_renew_trim_whitespace(char **p_this);

/* Create a string that is a subset of another.
 */
void string_new_substring(
    char **p_this,
    char *other,
    ulong first_index,
    ulong length);

/* Join an array of strings using the given 'join' string between them.
 */
void string_new_join(char **p_this, ulong n, char **others, char *join);

/* Replace any of the characters in the 'find' string with the 'replace'
 * character.
 */
void string_new_replace_chars(
    char **p_this,
    char *other,
    char *find,
    char replace);

/* Create a new string, replacing the given 'find' character with the given
 * 'replace' string. To escape the 'find' character, double it.
 */
void string_new_replace_char_with_string(
    char **p_this,
    char *other,
    char find,
    char *replace);

/* Create a new string, performing standard C-escaping on the supplied string.
 */
void string_new_escape(char **p_this, char *other);

/* Escape a string, except newlines and backslashes. Used for logging and
 * exception throwing, to avoid sending non-printing characters to the console.
 */
void string_new_escape_for_logging(char **p_this, char *other);

/* Escape a string in-place, except newlines and backslashes.
 */
void string_renew_escape_for_logging(char **p_this);

/* Create a new string, performing standard C-escaping on a single character.
 */
void string_new_escape_char(char **p_this, char c);

/* Create a new string, undoing any standard C-escaping in the supplied string.
 */
void string_new_unescape(char **p_this, char *other);

/* Create a new string that is either 'other' with single quotes around it, or
 * else 'null_string' if 'other' is null.
 */
void string_new_quote_or_null(char **p_this, char *other, char *null_string);

/* Make a new string that is a copy of the original string, but with numerical
 * separators added in, according to the following rules:
 *
 * - A decimal number is any number starting with a non-zero decimal digit,
 *   possibly with a decimal place followed by more decimal digits.
 *
 * - A hexidecimal number is any number starting with 0x or 0X followed by some
 *   number of hex digits, matching the case of the 'X' for letters.
 *
 * - Any decimal number with more than four integral digits has thousands
 *   separators inserted.
 *
 * - Any decimal number with more than four decimal digits has spaces inserted
 *   every three digits after the decimal point.
 *
 * - Any hexadecimal number with more than four hex digits has spaces inserted
 *   every four hex digits, back from the end of the number.
 *
 * This function, and the next, are more convenient for "retrofitting" commas
 * to the output of log() and throw() without needing to create and destroy
 * strings.
 */
void string_new_add_separators(char **p_this, char *other);

/* The same, in-place.
 */
void string_renew_add_separators(char **p_this);

/* Renew the allocation of a string to be just sufficient for the actual length
 * of string contained within it.
 */
void string_renew_trim_length(char **p_this, ulong old_length);

/* Append a single string to another.
 */
void string_renew_append(char **p_this, char *other);

/* Append any number of strings to an existing string, renewing its allocation.
 * Each vararg must be of type char *. The last argument must be a null
 * sentinel.
 */
void string_renew_append_until_null(char **p_this, ...);

/* Append a single character to an existing string, renewing its allocation.
 */
void string_renew_append_char(char **p_this, char c);

/* Append a formatted string to a string, renewing its allocation.
 */
void string_renew_append_f(char **p_this, char *fmt, ...);

/* Append a formatted string to a string, from a va_list.
 */
void string_renew_append_vf(
    char **p_this,
    char *fmt,
    va_list ap1,
    va_list ap2);

/* Insert a string at the start of the string.
 */
void string_renew_prepend(char **p_this, char *prefix);

/* Prepend with format.
 */
void string_renew_prepend_f(char **p_this, char *fmt, ...);

/* If the string does not end with the given string, add it to the end.
 */
void string_renew_ensure_ends_with(
    char **p_this,
    char *other,
    bool ignore_case);

/* Shorthand for deleting a string and creating a new one that is a copy of
 * another.
 */
void string_renew_copy(char **p_this, char *other);

/* Shorten a string by n characters. Fails with error if n is greater than the
 * length of the string.
 */
void string_renew_shorten(char **p_this, ulong n);

/* Shorten a string by n characters at each end. Fails with error if n is
 * greater than half the length of the string.
 */
void string_renew_shorten_both_ends(char **p_this, ulong n);

/* Shorten a string by n characters by removing from the start of the string
 * rather than the end. Fails with error if n is greater than the length of the
 * string.
 */
void string_renew_shorten_from_start(char **p_this, ulong n);

/* Indent a string by adding n lots of a standard indent string (by default,
 * ". ") at the start of every line.
 */
void string_renew_indent(char **p_this, ulong n);

/* Construct the array of strings obtained by splitting a string on a given
 * character.
 */
void strings_new_split(
    char ***p_this,
    ulong *num_results,
    char *other,
    char split_char);

/* Destructor.
 */
void string_delete(char **p_this);

/* Print.
 */
void string_print(char *this, struct Printer *p);

/* Save.
 */
void string_save(char *this, struct File *f);

/* More efficient than other methods.
 */
bool string_is_empty(char *this);

/* Length of a string.
 */
ulong string_length(char *this);

/* Less obscure than a direct call to strcmp.
 */
bool string_equals(char *this, char *other);

/* Comparator.
 */
sbyte string_compare(char *a, char *b, void *x);

/* Comparator for pointers.
 */
sbyte string_pointer_compare(char **a, char **b, void *x);

/* Whether the string contains any nonprintable characters.
 */
bool string_contains_nonprintable(char *this);

/* Whether the string contains the given character.
 */
bool string_contains_char(char *this, char c);

/* Whether the string contains any of the given characters.
 */
bool string_contains_chars(char *this, char *s);

/* Whether a string starts with a particular string.
 */
bool string_starts_with(char *this, char *other, bool ignore_case);

/* Whether a string ends with a particular string.
 */
bool string_ends_with(char *this, char *other, bool ignore_case);

/* First occurrence of a character in a string, or -1 if not found.
 */
long string_first_index_of(char *this, char c);

/* n-th (zero-offset) occurrence of a character, or -1 if not found.
 */
long string_nth_index_of(char *this, ulong n, char c);

/* Last occurrence of a character in a string, or -1 if not found.
 */
long string_last_index_of(char *this, char c);

/* n-th-last (zero-offset) occurrence of a character, or -1 if not found.
 */
long string_nth_last_index_of(char *this, ulong n, char c);

/* Determine how many times the given character appears in the string.
 */
ulong string_num_occurrences(char *this, char c);

/* Check that the substring [start, end) equals 'other'.
 */
bool string_substring_equals(char *this, ulong start, ulong end, char *other);

/* Whether the string contains the given string at the given index.
 */
bool string_contains_at(char *this, ulong index, char *other);

/* Replace any of the characters in the first string with the replacement
 * character.
 */
void string_replace_chars(char *this, char *find, char replace);

/* Trim trailing whitespace in a string, simply by moving back the '\0'. Use
 * string_renew_trim() to actually reallocate a shorter string.
 */
void string_trim_trailing_whitespace(char *this);

/* Create a new string that is a copy of a string, with line breaks inserted to
 * ensure that it fits within the given width. 'start' is the starting position
 * on the line (zero-offset), and 'indent' is how much to indent any lines
 * after the line break. Returns whether any wrapping occurred, rather than
 * this.
 */
void string_new_copy_wrap(
    char **p_this,
    char *other,
    ulong start,
    ulong width,
    ulong indent,
    bool *wrapped);

/* Renew a string, wrapping it to fit.
 */
void string_renew_wrap(
    char **p_this,
    ulong start,
    ulong width,
    ulong indent,
    bool *wrapped);

/* Append a wrapped string.
 */
void string_renew_append_wrap(
    char **p_this,
    char *other,
    ulong start,
    ulong width,
    ulong indent,
    bool *wrapped);

/* Create a string from printf-like format, wrapping it to fit.
 */
void string_new_f_wrap(
    char **p_this,
    ulong start,
    ulong width,
    ulong indent,
    bool *wrapped,
    char *fmt,
    ...);

/* Append a wrapped string with printf.
 */
void string_renew_append_f_wrap(
    char **p_this,
    ulong start,
    ulong width,
    ulong indent,
    bool *wrapped,
    char *fmt,
    ...);

/* Copy a string to an already-allocated block of memory. Used to avoid any
 * strcpy warnings. The length of the string (excluding final \0) must be no
 * greater than max_len. The pointer returns points to the final \0 written to
 * other, to allow writing of more to that buffer.
 */
char *string_copy(char *this, char *other, ulong max_len);

/* Useful for unit tests.
 */
ulong string_f_get_max_len(char *fmt, ...);

/* These functions are used by all _f functions, including those outside this
 * library, because vararg lists can't be rewound.
 */
ulong string_vf_get_max_len(char *fmt, va_list ap);
void string_vf_print_into(char *this, char *fmt, va_list ap, ulong max_len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
