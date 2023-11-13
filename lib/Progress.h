/* Object class for providing percentage complete log line feedback through a
 * Logger roughly every five seconds. The class is threadsafe, and avoids
 * requesting the system unixtime too often by adaptively adjusting the number
 * of counts between time checks, exponentially increasing or instantaneously
 * decreasing that count based on whether the actual time period between checks
 * is less than or greater than the target, which is arbitrarily set to a 10
 * us. (Checking the system time seems to take a handful to a dozen or two
 * nanoseconds on typical modern machines, so checking every ten us only causes
 * a maximum overhead of around 0.1%.)
 *
 * There are two modes:
 *
 * - Regular mode: The total number of operations is known ahead of time. In
 *   this mode, the feedback line is of the form "<x>% done ...", where enough
 *   decimal places are provided to ensure that <x> is different from the
 *   previous feedback line.
 *
 * - Initial mode: The total number of operations is not known ahead of time.
 *   In this mode, the feedback line is of the form "<n> <string> ...", where
 *   <n> is the number of operations that have happened, and <string> is
 *   supplied in the constructor, where separate singular and plural strings
 *   can be specified. The destructor prints a final "<n> <string> in total."
 *   to automatically provide feedback on the total number of operations,
 *   whether or not any other feedback lines were output.
 */
#ifndef PROGRESS_H
#define PROGRESS_H

#ifdef __cplusplus
extern "C" {
#endif

struct Progress;

#include "Logger.h"
#include "unixtime.h"

typedef struct Progress {
  char *_lastFeedback;
  char *_pluralString;
  char *_singularString;
  ulong count;
  ulong countPerCheck;
  ulong lastCheckCount;
  ulong numCallbacks;
  ulong totalCount;
  long lastCheckUs;
  long lastFeedbackUs;
  Lock lock;
  struct Logger *logger;
  void (**_callbacks)(char **p_this, ulong count, ulong total_count, void *x);
  void **_xs;
} Progress;

struct Printer;

/* Constructor for regular mode.
 */
void progress_new(Progress **p_this, struct Logger *logger, ulong total_count);

/* Constructor for unbounded mode, where there is either no bound to the count
 * or we don't yet know what it is.
 */
void progress_new_unbounded(
    Progress **p_this,
    struct Logger *logger,
    char *singular_string,
    char *plural_string);

/* Destructor.
 */
void progress_delete(Progress **p_this);

/* Print.
 */
void progress_print(Progress *this, struct Printer *p);

/* Add a callback function that is called whenever progress will be printed.
 * The callback must create a string in *p_this that will be added before the
 * ellipsis. Leading and trailing spaces do not need to be added. Any number of
 * callbacks can be added.
 */
void progress_add_callback(
    Progress *this,
    void (*callback)(char **p_this, ulong count, ulong total_count, void *x),
    void *x);

/* Signal that the next operation (of the total count) has completed.
 */
void progress_bump(Progress *this);

/* Signal that multiple operations have completed.
 */
void progress_bump_multi(Progress *this, ulong count);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
