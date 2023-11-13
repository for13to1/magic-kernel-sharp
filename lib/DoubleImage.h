/* Internal floating-point representation of an RGBA, RGB, YA, or Y image,
 * where Y is grayscale. Full intensity is defined to have a value of 1.0. The
 * A channel, if present, is premultiplied. Unlike ByteImage, only the channels
 * actually present are allocated.
 */
#ifndef DOUBLE_IMAGE_H
#define DOUBLE_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "srgb.h"

typedef struct {
  bool gamma;
  byte numChannels;
  ulong width;
  ulong height;
  double **r;
  double **g;
  double **b;
  double **a;
  double **y;
  double ***_channels;
} DoubleImage;

/* Create a new image, specifying the number of channels: 4 for RGBA, 3 for
 * RGB, 2 for YA, and 1 for Y.
 */
void double_image_new(
    DoubleImage **p_this,
    ulong width,
    ulong height,
    byte num_channels);

/* Create a new color DoubleImage with an alpha channel, with all pixels set to
 * opaque black (red, green, and blue set to 0; alpha set to 1).
 */
void double_image_new_rgba(DoubleImage **p_this, ulong width, ulong height);

/* Create a new color DoubleImage with no alpha channel, with all pixels set to
 * black (red, green, and blue set to 0).
 */
void double_image_new_rgb(DoubleImage **p_this, ulong width, ulong height);

/* Create a new grayscale DoubleImage with an alpha channel, with all pixels
 * set to opaque black (y set to 0, alpha set to 1).
 */
void double_image_new_ya(DoubleImage **p_this, ulong width, ulong height);

/* Create a new grayscale DoubleImage with no alpha channel, with all pixels
 * set to black (y set to 0).
 */
void double_image_new_y(DoubleImage **p_this, ulong width, ulong height);

/* Split a DoubleImage into single-channel DoubleImages. Returns the number of
 * channels.
 */
byte double_images_new_split_channels(
    DoubleImage ***p_this,
    DoubleImage *other);

/* Reconstruct a DoubleImage from its channels.
 */
void double_image_new_from_channels(
    DoubleImage **p_this,
    DoubleImage **channels,
    byte num_channels);

/* Split a DoubleImage with transparency into its alpha and non-alpha images.
 */
void double_images_new_split_alpha(
    DoubleImage **p_alpha,
    DoubleImage **p_non_alpha,
    DoubleImage *other);

/* Create a new DoubleImage by adding an alpha image to a non-alpha image.
 */
void double_image_new_add_alpha(
    DoubleImage **p_this,
    DoubleImage *alpha,
    DoubleImage *non_alpha);

/* Create a new DoubleImage that is a copy of another.
 */
void double_image_new_copy(DoubleImage **p_this, DoubleImage *other);

/* Create a new DoubleImage that is the transpose of an existing one.
 */
void double_image_new_transpose(DoubleImage **p_this, DoubleImage *other);

/* Create a DoubleImage that is a crop of another.
 */
void double_image_new_crop(
    DoubleImage **p_this,
    DoubleImage *other,
    ulong x_offset,
    ulong new_width,
    ulong y_offset,
    ulong new_height);

/* Createa a new DoubleImage that is the "uncrop" of another, i.e., expand its
 * canvas, filling the extra pixels with the supplied background, which must
 * have four components (RGBA). If the components of the background imply color
 * or alpha and the input image don't have either of those, the output is
 * promoted to have those. This constructor is actually a generalized crop: it
 * can handle any combination of canvas expansion and cropping.
 */
void double_image_new_uncrop(
    DoubleImage **p_this,
    DoubleImage *other,
    long x_offset,
    ulong new_width,
    long y_offset,
    ulong new_height,
    double *background);

/* Add a DoubleImage to another.
 */
void double_image_new_add(
    DoubleImage **p_this,
    DoubleImage *a,
    DoubleImage *b);

/* Subtract a DoubleImage from another.
 */
void double_image_new_subtract(
    DoubleImage **p_this,
    DoubleImage *a,
    DoubleImage *b);

/* Add a DoubleImage to another in quadrature, i.e., the result for each
 * channel of each pixel is the square root of the sum of the squares.
 */
void double_image_new_add_quadrature(
    DoubleImage **p_this,
    DoubleImage *a,
    DoubleImage *b);

/* Destructor.
 */
void double_image_delete(DoubleImage **p_this);

/* Print.
 */
void double_image_print(DoubleImage *this, Printer *p);

/* Whether a DoubleImage has an alpha channel.
 */
bool double_image_has_alpha(DoubleImage *this);

/* Whether a DoubleImage is grayscale.
 */
bool double_image_is_grayscale(DoubleImage *this);

/* Whether the alpha channel contains any values that are not 1.0.
 */
bool double_image_contains_transparency(DoubleImage *this);

/* The maximum value of any RGB element in a DoubleImage.
 */
double double_image_max_rgb(DoubleImage *this);

/* The maximum absolute value of any RGB element in a DoubleImage.
 */
double double_image_max_abs_rgb(DoubleImage *this);

/* The mean absolute value of all RGB elements in a DoubleImage.
 */
double double_image_mean_abs_rgb(DoubleImage *this);

/* Make the image fully opaque (the alpha for all pixels set to 1.0).
 */
void double_image_make_opaque(DoubleImage *this);

/* Bound each value of each channel to the given range.
 */
void double_image_bound(DoubleImage *this, double min, double max);

/* Automatically scale the image so that the minimum value of any channel is
 * zero, and the maximum value of any channel is 1. Not valid if the image has
 * an alpha channel.
 */
void double_image_auto_scale(DoubleImage *this);

/* Scale the image so that the new value is m times the old value plus c.
 */
void double_image_scale(DoubleImage *this, double m, double c);

/* Scale the RGB channels of a DoubleImage.
 */
void double_image_scale_rgb(DoubleImage *this, double factor);

/* Convert an image, in place, between gamma and linear spaces.
 */
void double_image_linear_to_gamma(DoubleImage *this);
void double_image_gamma_to_linear(DoubleImage *this);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
