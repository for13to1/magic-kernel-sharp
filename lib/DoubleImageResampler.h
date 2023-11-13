/* Class for resampling a DoubleImage using Magic Kernel Sharp. Can be used for
 * same-size and enlarging (using a DoubleImageInterpolator) as well as for
 * reducing, done directly in this class. Pixels outside the original image are
 * taken to have the values specified by the background array, which must
 * supply four elements (RGBA).
 */
#ifndef DOUBLE_IMAGE_RESAMPLER_H
#define DOUBLE_IMAGE_RESAMPLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "DoubleImageInterpolator.h"

typedef struct {
  bool mightReduce;
  byte mksA;
  byte mksV;
  byte numChannels;
  ulong width;
  ulong height;
  double *_background;
  DoubleImage *_image;
  DoubleImageInterpolator *_interpolator;
} DoubleImageResampler;

/* Constructor. The flag might_reduce must be set to be true if the class could
 * possibly be used for reductions. (This makes the class more efficient for
 * cases in which it won't be used for reductions.)
 */
void double_image_resampler_new(
    DoubleImageResampler **p_this,
    DoubleImage *image,
    double *background,
    bool might_reduce,
    byte mks_a,
    byte mks_v);

/* Destructor.
 */
void double_image_resampler_delete(DoubleImageResampler **p_this);

/* Print.
 */
void double_image_resampler_print(DoubleImageResampler *this, Printer *p);

/* Get the resampled value at a particular position. The scale factors for the
 * x and y directions need to be supplied, since for reductions they are used
 * to determine the widths of the resampling kernels.
 */
void double_image_resampler_get(
    DoubleImageResampler *this,
    double x,
    double y,
    double k_x,
    double k_y,
    double *values);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
