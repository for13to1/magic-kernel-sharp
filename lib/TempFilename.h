/* Object class for temporary filenames, properly implemented this time (2023),
 * not like the 2014 version! Uses a lock file to ensure atomicity. Removes the
 * temp file (if it exists, unless told otherwise) as well as the lock file
 * when the object is destroyed.
 */
#ifndef TEMP_FILENAME_H
#define TEMP_FILENAME_H

#ifdef __cplusplus
extern "C" {
#endif

struct TempFilename;

#include "ExternalFile.h"
#include "Random.h"
#include "directory.h"

typedef struct TempFilename {
  bool keep;
  char *_filename;
  char *_lockFilename;
  struct ExternalFile *_lockFile;
} TempFilename;

/* Constructor.
 */
void temp_filename_new(TempFilename **p_this, char *extension);

/* Constructor specifying whether the destructor should keep the file, rather
 * than trying to remove it.
 */
void temp_filename_new_keep(TempFilename **p_this, char *extension, bool keep);

/* Destructor. Closes and removes the lock file, and tries to remove any file
 * with the specified filename, if it exists, unless told to keep above.
 */
void temp_filename_delete(TempFilename **p_this);

/* Release the lock file. You should do this as soon as you have created the
 * file with the given this->_filename, or else you will be holding files open
 * for the locks just because you are keeping the filename. You can call this
 * multiple times without problem.
 */
void temp_filename_unlock(TempFilename *this);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
