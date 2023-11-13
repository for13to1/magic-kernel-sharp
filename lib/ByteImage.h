/* Generic RGBA images with eight bits per channel per pixel, assumed to be in
 * the sRGB colorspace. The alpha channel, if used, is "straight," i.e. not
 * premultipled, as this is the standard for eight-bit image file formats.
 * Loading from or saving to PNG and JPEG files is done in png_file and
 * jpeg_file respectively. Libraries that filter or resize these images first
 * convert them to DoubleImages.
 */
#ifndef BYTE_IMAGE_H
#define BYTE_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "DoubleImage.h"
#include "byte.h"
#include "parallel.h"
#include "srgb.h"

typedef struct {
  byte r;
  byte g;
  byte b;
  byte a;
} ByteImagePixel;

typedef struct {
  byte y;
  byte cb;
  byte cr;
  byte a;
} ByteImageYccaPixel;

typedef struct {
  bool ycc;
  ulong width;
  ulong height;
  ByteImagePixel **_pixels;
} ByteImage;

/* Constructor. All pixels are set to opaque black (red, green, and blue set to
 * 0; alpha set to 255).
 */
void byte_image_new(ByteImage **p_this, ulong width, ulong height);

/* Constructor, with all pixels set to transparent black (red, green, blue, and
 * alpha all set to 0).
 */
void byte_image_new_transparent(
    ByteImage **p_this,
    ulong width,
    ulong height);

/* Copy.
 */
void byte_image_new_copy(ByteImage **p_this, ByteImage *other);

/* Load.
 */
void byte_image_new_load(ByteImage **p_this, File *f);

/* Create a new ByteImage from a DoubleImage.
 */
void byte_image_new_from_double(ByteImage **p_this, DoubleImage *other);

/* Create a new ByteImage from a DoubleImage diff, encoding as per JPEG-Clear.
 */
void byte_image_new_from_double_diff(ByteImage **p_this, DoubleImage *diff);

/* Destructor.
 */
void byte_image_delete(ByteImage **p_this);

/* Print.
 */
void byte_image_print(ByteImage *this, Printer *p);

/* Save.
 */
void byte_image_save(ByteImage *this, File *f);

/* Equals.
 */
bool byte_image_equals(ByteImage *this, ByteImage *other);

/* Create a new DoubleImage from a ByteImage.
 */
void double_image_new_from_byte(DoubleImage **p_this, ByteImage *other);

/* Create a new DoubleImage from a ByteImage encoded as per JPEG-Clear.
 */
void double_image_new_from_byte_diff(
    DoubleImage **p_this,
    ByteImage *other);

/* Make the image fully opaque (the alpha for all pixels set to 255).
 */
void byte_image_make_opaque(ByteImage *this);

/* Set all pixels in an image to the given pixel value.
 */
void byte_image_clear(ByteImage *this, ByteImagePixel pixel);

/* Accessors.
 */
ByteImagePixel byte_image_get(ByteImage *this, ulong i, ulong j);
byte byte_image_get_r(ByteImage *this, ulong i, ulong j);
byte byte_image_get_g(ByteImage *this, ulong i, ulong j);
byte byte_image_get_b(ByteImage *this, ulong i, ulong j);
byte byte_image_get_a(ByteImage *this, ulong i, ulong j);
byte byte_image_get_y(ByteImage *this, ulong i, ulong j);

void byte_image_set(ByteImage *this, ulong i, ulong j, ByteImagePixel pixel);
void byte_image_set_r(ByteImage *this, ulong i, ulong j, byte r);
void byte_image_set_g(ByteImage *this, ulong i, ulong j, byte g);
void byte_image_set_b(ByteImage *this, ulong i, ulong j, byte b);
void byte_image_set_a(ByteImage *this, ulong i, ulong j, byte a);
void byte_image_set_y(ByteImage *this, ulong i, ulong j, byte y);

/* Accessor when the image has been loaded in YCbCr space.
 */
ByteImageYccaPixel byte_image_get_ycca(ByteImage *this, ulong i, ulong j);

/* Whether the image contains any transparency. If it doesn't, many operations
 * are simplified.
 */
bool byte_image_contains_transparency(ByteImage *this);

/* Whether a byte image is grayscale.
 */
bool byte_image_is_grayscale(ByteImage *this);

/* Pixel convenience functions.
 */
ByteImagePixel byte_image_pixel_from_rgba_hex(uint hex);
ByteImagePixel byte_image_pixel_opaque_from_rgb_hex(uint hex);
ByteImagePixel byte_image_pixel_opaque_from_rgb_hex_string(char *hex);
ByteImagePixel byte_image_pixel_opaque_from_grayscale(byte gray);
ByteImagePixel byte_image_pixel_opaque_black(void);
ByteImagePixel byte_image_pixel_from_hsl(double h, double s, double l);

bool byte_image_pixel_equals(ByteImagePixel x, ByteImagePixel y);

/* YCCA pixel conversions.
 */
ByteImageYccaPixel byte_image_ycca_pixel_from_rgba(ByteImagePixel pixel);
ByteImagePixel byte_image_pixel_from_ycca(ByteImageYccaPixel ycca);

/* Convert a YCCA image to RGBA, in-place.
 */
void byte_image_convert_ycca_to_rgba(ByteImage *this);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
