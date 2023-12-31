/* Static library providing some simple methods for spawning shell commands,
 * either blocking or non-blocking, and optionally capturing stdout and/or
 * stderr.
 */
#ifndef SPAWN_H
#define SPAWN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "TempFilename.h"

/* This spawns a 'simple' command: it is blocking, and stdout and stderr are
 * not redirected.
 */
void spawn_simple(char *command, byte retries);

int try_spawn_simple(char *command);

/* This spawns a system command. Its stdout and stderr are redirected into
 * files with temp filenames. If p_err_filename is not null, then the stderr
 * filename is donated to it; likewise with p_out_filename. If 'blocking' is
 * true, the program blocks until the command has completed; if it is false,
 * the command is detached and runs independently and asynchronously. In that
 * case, if p_completed_filename is not null, the name of the file touched when
 * execution is complete will be donated to it. try_spawn() returns the exit
 * code from the command; spawn() exits with throw() if the exit code is
 * nonzero.
 */
void spawn(char *command,
    bool blocking,
    char **p_out_filename,
    char **p_err_filename,
    char **p_completed_filename);

int try_spawn(char *command,
    bool blocking,
    char **p_out_filename,
    char **p_err_filename,
    char **p_completed_filename);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
