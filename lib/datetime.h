/* Static library providing functions to print the current datetime to a
 * string, either local time or UTC, in ISO-8601 hierarchy of fields (so that
 * string comparisons are equilavent to datetime comparisons), with almost
 * every option configurable.
 */

#ifndef DATETIME_H
#define DATETIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "unixtime.h"
#include <sys/time.h>

/* Print the current time to a string, either local or UTC.
 *
 *   utc: true for UTC, or false for local time
 *   date_sep: separator between date parts (typical: "-" or "")
 *   t_sep: separator between the date and time (typical: " " or "T")
 *   time_sep: separator between time parts (typical: ":" or "")
 *   decimal_sep: separator between time and microseconds (typical: "." or "")
 *   do_date: include the date
 *   do_time: include the time
 *   do_minutes: include minutes in the time
 *   do_seconds: include seconds in the time
 *   decimal_places: include this number of decimal places of seconds (max 6)
 */
void string_new_datetime_current(
    char **p_this,
    bool utc,
    char *date_sep,
    char *t_sep,
    char *time_sep,
    char *decimal_sep,
    bool do_date,
    bool do_time,
    bool do_minutes,
    bool do_seconds,
    byte decimal_places);

/* As above, but supplying the timeval rather than taking the current time.
 * Useful for unit tests.
 */
void string_new_datetime_from_timeval(
    char **p_this,
    struct timeval *tv,
    bool utc,
    char *date_sep,
    char *t_sep,
    char *time_sep,
    char *decimal_sep,
    bool do_date,
    bool do_time,
    bool do_minutes,
    bool do_seconds,
    byte decimal_places);

/* The standard ANSI C library functions gmtime() and localtime() are not
 * thread-safe, because they use static memory. This function wraps that for
 * all calls in this codebase with a lock to ensure thread-safety, and copies
 * the tm structure's value out before releasing the lock. As elsewhere, if utc
 * is true then gmtime() is called; if utc is false then localtime() is called.
 */
struct tm time_t_to_tm(time_t *timer, bool utc);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
