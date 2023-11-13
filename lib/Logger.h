/* Object class providing convenient slog() and flog() for matched and nested
 * logging of "Starting..." and "Finished..." messages, together with any
 * unmatched (single) "random" log lines with rlog(). Each object is threadsafe
 * for rlog(); slog() and flog() should not be called by more than one thread
 * for the same Logger.
 */
#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

struct Logger;

#include "Lock.h"
#include "log.h"

typedef struct Logger {
  bool running;
  char **_messages;
  ulong indent;
  ulong numMessages;
  ulong maxMessages;
  Lock lock;
  FILE *ansifile;
} Logger;

struct Printer;

/* Constructor. If "ansifile" is null, logging is suppressed.
 */
void logger_new(Logger **p_this, FILE *ansifile);

/* Destructor.
 */
void logger_delete(Logger **p_this);

/* Print.
 */
void logger_print(Logger *this, struct Printer *p);

/* Start log, which will print the string in square brackets and indent.
 */
void slog(Logger *this, char *fmt, ...);

/* Finish log, which will print the matching start string in square brackets
 * with a leading slash, and undo the indent
 */
void flog(Logger *this);

/* "Random" (single) log line. Indented according to the current level of
 * nested slog() commands, but does not itself affect nesting or indent levels.
 * Note that no final punctuation is printed, as it may need to be a colon
 * rather than a period, or maybe no punctuation at all.
 */
void rlog(Logger *this, char *fmt, ...);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
