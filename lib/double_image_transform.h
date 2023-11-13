/* Static library providing methods for transforming a DoubleImage using
 * DoubleImageResampler.
 */
#ifndef DOUBLE_IMAGE_TRANSFORM_H
#define DOUBLE_IMAGE_TRANSFORM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "DoubleImageResampler.h"
#include "DoubleImageTransformer.h"
#include "shuffle.h"

/* Transform an image using Magic Kernel Sharp resampling. The functions
 * in_to_out and out_to_in must provide the (invertible) transforms between
 * input and output coordinates, which are floating-point versions of the
 * standard pixel center locations, i.e. integers [0, n). Pixels outside the
 * original image are taken to have the values specified by the 'background'
 * array, which must have four components; if these values imply color and/or
 * alpha, and the input image is missing either, then the number of channels in
 * the input image is implicitly promoted to handle these properties. A margin
 * of 'margin' pixels is added to surround the original image. The enlarge,
 * reduce, and same_size flags specify whether any of these might be needed.
 */
DoubleImage *double_image_new_transform(
    DoubleImage **p_this,
    DoubleImage *other,
    DoubleImageTransformer *transformer,
    double *background,
    ulong margin,
    bool might_reduce,
    byte mks_a,
    byte mks_v);

/* Rotate, resize, and shift an image using the above transformer.
 */
DoubleImage *double_image_new_rotate_resize_shift(
    DoubleImage **p_this,
    DoubleImage *other,
    double degrees,
    double scale_factor,
    double x_shift,
    double y_shift,
    double *background,
    ulong margin,
    byte mks_a,
    byte mks_v);

/* Change the perspective of an image by changing the direction of view.
 */
DoubleImage *double_image_new_change_perspective(
    DoubleImage **p_this,
    DoubleImage *other,
    double field_of_view_degrees,
    double dov_shift_pixels_x,
    double dov_shift_pixels_y,
    double rotation_degrees,
    double scale_factor,
    double x_shift,
    double y_shift,
    double *background,
    ulong margin,
    byte mks_a,
    byte mks_v);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
