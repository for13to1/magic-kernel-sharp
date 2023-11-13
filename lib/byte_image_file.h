/* Simple static library for reading images from and writing images to standard
 * PNG and JPEG files.
 */
#ifndef BYTE_IMAGE_FILE_H
#define BYTE_IMAGE_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jpeg_file.h"
#include "png_file.h"

#define BYTE_IMAGE_FILETYPE_PNG "png"
#define BYTE_IMAGE_FILETYPE_JPEG "jpeg"

/* Load an image. Will automatically detect whether the file is PNG or JPEG,
 * regardless of its filename or extension.
 */
void byte_image_new_load_file(ByteImage **p_this, char *filename);

/* Save an image. Will automatically choose whether to use PNG or JPEG format
 * based on the file extension. If JPEG, default quality is used.
 */
void byte_image_save_file(ByteImage *this, char *filename);

/* Save an image, Will automatically choose whether to use PNG or JPEG format
 * based on the file extension. If JPEG, the specified quality is used.
 */
void byte_image_save_quality(ByteImage *this, char *filename, byte quality);

/* Save an image, specifying both the filetype and, if JPEG, the quality.
 */
void byte_image_save_type_quality(
    ByteImage *this,
    char *filename,
    char *filetype,
    byte quality);

/* Save a DoubleImage to a file.
 */
void double_image_save_file(DoubleImage *this, bool auto_scale, char *fmt, ...);

/* Returns a string specifying the filetype of the given file, which must
 * exist. Will be BYTE_IMAGE_FILETYPE_PNG, BYTE_IMAGE_FILETYPE_JPEG, or null if
 * not recognized as either of these formats.
 */
char *byte_image_filetype(char *filename);

/* Returns a string specifying the filetype of the given file, from the
 * filename only. Will be BYTE_IMAGE_FILETYPE_PNG, BYTE_IMAGE_FILETYPE_JPEG, or
 * null if not recognized as either of these formats.
 */
char *byte_image_filetype_from_filename(char *filename);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
