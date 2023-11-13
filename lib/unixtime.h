/* Convenience static library supplying the current unixtime.
 */
#ifndef UNIXTIME_H
#define UNIXTIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jpc.h"
#include <sys/time.h>

/* The current unixtime, in microseconds.
 */
long unixtime_current_us(void);

/* The current unixtime, in seconds, correct to the microsecond.
 */
double unixtime_current(void);

/* Construct a human-readable string representing a time interval, specified by
 * the length of the interval in microseconds, which in most cases consisting
 * of five UTF-8-encoded characters. Some examples, taken from the unit tests:
 *
 *   00000 001μs 009μs 010μs 011μs 099μs 100μs 101μs 999μs 1.0ms 9.9ms 010ms
 *   011ms 023ms 099ms 100ms 101ms 999ms 1.00s 1.01s 1.23s 9.99s 10.0s 10.1s
 *   12.3s 59.9s 1.00m 1.01m 1.09m 1.10m 2.34m 9.99m 10.0m 59.9m 1.00h 9.99h
 *   10.0h 23.9h 1.00d 0365d 1.00y 12.3y 0456y 7891y 12345y
 *
 * Notes:
 * - No space character appears in any string constructed by this function.
 * - The 'μ' in 'μs' is actually a two-byte UTF-8 character "\xce\xbc", so that
 *   strings representing microseconds are actually of length six bytes.
 * - If 10,000 or more years are specified, the full number of years will be
 *   printed, followed by 'y', which yields a string longer than five UTF-8
 *   characters. (This should be a case that never occurs in practice.)
 *
 * If 'us' appears to be a ulong cast of a negative long, the function will
 * throw an exception.
 */
void string_new_time_interval(char **p_this, ulong us);

/* Same, but the 'μ' in 'μs' is replaced by 'u' for 'us' so that escaping of
 * the string is avoided.
 */
void string_new_time_interval_safe(char **p_this, ulong us);

/* Same as the above, but from a given then_us to a given now_us which cannot
 * be earlier. than 'then'. A nonpositive then_us signals that it has not yet
 * been initialized, and the time interval will be set to zero.
 */
void string_new_time_elapsed(char **p_this, long now_us, long then_us);

/* Same, with 'μ' replaced by 'u'.
 */
void string_new_time_elapsed_safe(char **p_this, long now_us, long then_us);

/* Same as above, but from *last_us to the current time; *last_microsconds is
 * then updated with the current time. Any lock on last_us needs to be
 * performed by the calling code.
 */
void string_new_time_since(char **p_this, long *last_us);

/* Same, with 'μ' replaced by 'u'.
 */
void string_new_time_since_safe(char **p_this, long *last_us);

/* Progress callback with time remaining to a deadline, which must be a long
 * unixtime, the pointer to which must be passed as 'x'.
 */
void deadline_progress(char **p_this, ulong count, ulong total_count, void *x);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
