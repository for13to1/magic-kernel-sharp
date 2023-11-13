/* Static library providing methods for reading images from and writing images
 * to standard PNG files.
 */
#ifndef PNG_FILE_H
#define PNG_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "File.h"
#include "ByteImage.h"

/* Load a ByteImage from a PNG file.
 */
void byte_image_new_load_png(ByteImage **p_this, char *filename);

/* Save a ByteImage as a PNG file.
 */
void byte_image_save_png(ByteImage *this, char *filename);

/* Determine whether a file is a PNG image.
 */
bool file_is_png(char *filename);

/* Return the dimensions of the PNG file, without loading the entire image.
 */
void png_file_get_dimensions(char *filename, ulong *width, ulong *height);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
