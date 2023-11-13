/* Static library for reading images from and writing images to standard JPEG
 * files.
 */
#ifndef JPEG_FILE_H
#define JPEG_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "File.h"
#include "ByteImage.h"

/* Load a JPEG file into a new ByteImage in the standard RGB colorspace, using
 * fancy upsampling (the original Magic Kernel!) for the chrominance channels
 * as is standard.
 */
void byte_image_new_load_jpeg(ByteImage **p_this, char *filename);

/* Load a JPEG file into a new ByteImage in YCC space, with no fancy upsampling
 * of the chrominance.
 */
void byte_image_new_load_jpeg_ycc(ByteImage **p_this, char *filename);

/* Load a JPEG file into a new ByteImage, specififying whether the colorspace
 * is YCC or standard RGB.
 */
void byte_image_new_load_jpeg_colorspace(
    ByteImage **p_this,
    char *filename,
    bool ycc);

/* Save a ByteImage to a JPEG file with the specified quality (0-100).
 */
void byte_image_save_jpeg(ByteImage *this, char *filename, byte quality);

/* Check whether a file is a JPEG file by trying to read its header.
 */
bool file_is_jpeg(char *filename);

/* Get the current default quality setting. This is not used within the library
 * itself, but calling programs can use it as a sensible default.
 */
byte jpeg_file_get_default_quality(void);

/* Set the default quality to be used for saving. Returns the previous value.
 */
byte jpeg_file_set_default_quality(byte default_quality);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
