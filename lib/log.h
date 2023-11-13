/* Static library providing simple logging functionality with printf-type
 * formatting. alog() logs to an arbitrary ANSI FILE; olog() logs to stdout;
 * and elog() logs to stderr.
 *
 * A complete log line (which may run over multiple lines) is logged in each
 * function call, to allow for thread-safe interleaving of complete log lines
 * from multiple threads. This means that the log line does not need to be
 * terminated with a newline.
 *
 * By default, each log line is prepended with the current local time, down to
 * the minute, as well as the time elapsed since the last log line, in a format
 * that covers the full range of microseconds to years. This can also be turned
 * off, and it can also be switched to UTC instead of local time (in which case
 * the times are appended with 'Z', for Zulu time). An indent level must always
 * be specified, which matches the indent used by dprint().
 *
 * A special debug logging command dlog() is also provided, which logs to
 * stderr but also includes the file and line after the timestamp (if the
 * latter has not been turned off), which can itself be turned off with the
 * macros DLOG_FILE_LINE_OFF and DLOG_FILE_LINE_ON. All logging from dlog() can
 * itself be suppressed or turned back on with the macros DLOG_OFF and DLOG_ON.
 *
 * A more powerful wrapper around this logging functionality is provided by the
 * object class Logger, which provides for automatically matched and indented
 * "Starting..." and "Finished..." logging, as well as random single-line log
 * lines.
 */
#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Lock.h"
#include "date.h"
#include "datetime.h"

/* Log to an arbitrary ANSI FILE.
 */
void alog(FILE *f, byte i, char *fmt, ...);

/* Log to stdout.
 */
void olog(byte i, char *fmt, ...);

/* Log to stderr.
 */
void elog(byte i, char *fmt, ...);

/* Log to stderr just prior to a throw, so suppress the timestamp.
 */
void tlog(char *fmt, ...);

/* Debug log.
 */
#define dlog \
  log_lock_dI(), \
  g_LOG_file_ = __FILE__, \
  g_LOG_line_ = __LINE__, \
  dlogI

void dlogI(byte i, char *fmt, ...);
void log_lock_dI(void);

/* Turn off debug logging, i.e. make it silently be ignored.
 */
#define DLOG_OFF g_LOG_on_ = false;

/* Turn debug logging back on.
 */
#define DLOG_ON g_LOG_on_ = true;

/* Turn off the printing of file names and line numbers in debug logging.
 */
#define DLOG_FILE_LINE_OFF g_LOG_fl_on_ = false;

/* Turn back on the printing of file names and line numbers in debug logging.
 */
#define DLOG_FILE_LINE_ON g_LOG_fl_on_ = true;

/* Whether to include timestamps. Defaults to true. Returns the previous value.
 * Does not apply to stderr logging, for which this is always turned on.
 */
bool log_timestamps(bool b);

/* Whether to use UTC rather than local time for the timestamps. Defaults to
 * false. Returns the previous value.
 */
bool log_utc(bool b);

/* Whether to automatically insert separators (thousands, decimals, hex) into
 * the logged strings. Turn off if you want to pipe stdout values to another
 * program. Defaults to true. Returns the previous value. Does not apply to
 * stderr logging, for which this is always turned on.
 */
bool log_separators(bool b);

/* Whether to automatically end log lines with newlines. Defaults to true.
 * Returns the previous value. Does not apply to stderr logging, for which this
 * is always turned on.
 */
bool log_newlines(bool b);

/* Whether to escape log lines, except for newlines. Defaults to true. Returns
 * the previous value. Does not apply to stderr logging, for which this is
 * always turned on.
 */
bool log_escape(bool b);

/* Whether to automatically log "raw," i.e. turn off timestamps, separators,
 * automatic newlines, and escaping. If raw is turned off, all of these turned
 * back on. Does not apply to stderr logging, for which this is always turned
 * on.
 */
void log_raw(bool b);

/* Log a newline to stdout. Only useful in raw mode.
 */
void ologn(byte i);

#ifndef LOG_C
extern bool g_LOG_on_;
extern bool g_LOG_fl_on_;
extern char *g_LOG_file_;
extern int g_LOG_line_;
#endif /* LOG_C */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
