/* Simple ISO-8601 date functionality, for 1901 through 2079. For simplicity,
 * dates are stored in immutable strings, in the YYYY-MM-DD format only.
 */
#ifndef DATE_H
#define DATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "numbers.h"

/* Validate a date, i.e. confirm that it is a valid ISO-8601 date in YYYY-MM-DD
 * format.
 */
void date_validate(char *date);
bool date_try_validate(char *date);

/* Parse a date into day, month, and year.
 */
void date_parse(char *date, byte *day, byte *month, ushort *year);
bool date_try_parse(char *date, byte *day, byte *month, ushort *year);

/* Get single properties of a date.
 */
byte date_day(char *date);
byte date_month(char *date);
char *date_month_shortname(char *date);
char *date_month_shortname_from_value(byte month);
char *date_month_longname(char *date);
char *date_month_longname_from_value(byte month);
ushort date_year(char *date);

/* Day of week methods, where 0 is Monday and 6 is Sunday.
 */
byte date_day_of_week(char *date);
char *date_day_of_week_shortname(char *date);
char *date_day_of_week_shortname_from_index(byte day_of_week);
char *date_day_of_week_longname(char *date);
char *date_day_of_week_longname_from_index(byte day_of_week);

/* The minimum and maximum dates handled by this library.
 */
char *date_min(void);
char *date_max(void);

/* Validate a date and then copy it into a new string.
 */
void string_new_date_copy(char **p_this, char *other);

/* Create a new string with the minimum or maximum date handled by this
 * library.
 */
void string_new_date_min(char **p_this);
void string_new_date_max(char **p_this);

/* Create a new string with today's or yesterday's date. If utc is true then
 * use the current date in UTC, otherwise use local time.
 */
void string_new_date_today(char **p_this, bool utc);
void string_new_date_yesterday(char **p_this, bool utc);

/* Create a new string with the date next after or previous to the given date.
 */
void string_new_date_next(char **p_this, char *date);
void string_new_date_previous(char **p_this, char *date);

/* Increment or decrement a date, in-place.
 */
void string_renew_date_increment(char **p_this);
void string_renew_date_decrement(char **p_this);

/* Create a new string, adding or subtracting a number of days from a given
 * date. The value added or subtracted can itself be negative.
 */
void string_new_date_add(char **p_this, char *date, int add);
void string_new_date_subtract(char **p_this, char *date, int subtract);

/* Increment or decrement a date by a number of days, in-place.
 */
void string_renew_date_add(char **p_this, int add);
void string_renew_date_subtract(char **p_this, int subtract);

/* Subtract a date from another, returning the number of days that date1 is
 * later than date2 (negative if it is before).
 */
int date_subtract(char *date1, char *date2);

/* Whether date1 is equal to date2.
 */
bool date_equals(char *date1, char *date2);

/* Whether date1 is earlier than date2.
 */
bool date_earlier(char *date1, char *date2);

/* Whether date1 is earlier than or equal to date2.
 */
bool date_earlier_or_equal(char *date1, char *date2);

/* Whether date1 is later than date2.
 */
bool date_later(char *date1, char *date2);

/* Whether date1 is later than or equal to date2.
 */
bool date_later_or_equal(char *date1, char *date2);

/* Increment or decrement a date in-place, copying into the existing string
 * rather than creating a new string. Again, this works because all dates have
 * the same memory allocation.
 */
void date_increment(char *this);
void date_decrement(char *this);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
