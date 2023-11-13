/* Object class implementing files stored in memory rather than being written
 * out to an external file system. This class, together with ExternalFile, are
 * wrapped in the overall File class.
 *
 * For efficiency, memory is allocated in "chunks." The class initially creates
 * an object with a 16-byte first chunk, to cater for the case when many of
 * these objects are created with small sizes. As this chunk is filled, it is
 * doubled in size, until it reaches 64 KiB. At that point a second 64 KiB
 * chunk is created. All successive chunks are 64 KiB in size.
 */
#ifndef MEMORY_FILE_H
#define MEMORY_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

struct MemoryFile;

#include "Swappable.h"
#include "bits.h"

typedef struct MemoryFile {
  byte *_bytes;
  ulong numBytes;
  ulong position;
  ulong size;
  struct SwappableValues *_swappable;
} MemoryFile;

/* Constructor. Note that no mode need be specify, as by definition all memory
 * files have all abilities.
 */
void memory_file_new(MemoryFile **p_this);

/* Destructor.
 */
void memory_file_delete(MemoryFile **p_this);

/* Print.
 */
void memory_file_print(MemoryFile *this, struct Printer *p);

/* Release the memory file. This disables any actions on it until it is
 * accessed again, and allows it to be swapped out if necessary.
 */
void memory_file_release(MemoryFile *this);

/* Reacquire a memory file. This swaps it back into memory, if necessary, and
 * re-enables it for actions.
 */
void memory_file_reacquire(MemoryFile *this);

/* Write a byte to a memory file.
 */
void memory_file_write_byte(MemoryFile *this, byte b);

/* Write out num_bytes bytes to a memory file.
 */
void memory_file_write_bytes(MemoryFile *this, byte *bs, ulong num_bytes);

/* Write out a matrix of bytes to a memory file.
 */
void memory_file_write_bytesm(
    MemoryFile *this,
    byte **bm,
    ulong n0,
    ulong num_bytes);

/* Read a byte from a memory file.
 */
byte memory_file_read_byte(MemoryFile *this);

/* Try to read a byte from a memory file.
 */
bool memory_file_try_read_byte(MemoryFile *this, byte *b);

/* Read num_bytes bytes from a memory file.
 */
void memory_file_read_bytes(MemoryFile *this, byte *bs, ulong num_bytes);

/* Read a matrix of bytes from a memory file.
 */
void memory_file_read_bytesm(
    MemoryFile *this,
    byte **bm,
    ulong n0,
    ulong num_bytes);

/* Get the current size of a memory file.
 */
ulong memory_file_get_size(MemoryFile *this);

/* Get the current position of a memory file.
 */
ulong memory_file_get_position(MemoryFile *this);

/* Whether a memory file is at its end, i.e. it has all been read.
 */
bool memory_file_is_at_end(MemoryFile *this);

/* Set the position in a memory file.
 */
void memory_file_set_position(MemoryFile *this, ulong position);

/* Rewind a memory file.
 */
void memory_file_rewind(MemoryFile *this);

/* Fast-forward a memory file, i.e. move to the end of the file.
 */
void memory_file_fast_forward(MemoryFile *this);

/* Whether the contents of a memory file equals that of the given byte blob.
 */
bool memory_file_contents_equals_blob(MemoryFile *this, ulong len, byte *blob);

/* Assert that the contents of a memory file equals that of the given byte
 * blob.
 */
void memory_file_assert_contents_equals_blob(
    MemoryFile *this,
    ulong len,
    byte *blob);

/* Create a blob with the contents of the memory file. Returns the length of
 * the blob.
 */
ulong blob_new_memory_file_contents(byte **p_this, MemoryFile *mf);

/* Create a string with the contents of the memory file.
 */
void string_new_memory_file_contents(char **p_this, MemoryFile *mf);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
