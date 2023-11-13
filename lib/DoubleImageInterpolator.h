/* Class for interpolating a DoubleImage using Magic Kernel Sharp. Can be used
 * for operations at the same size (e.g. rotations) and enlarging (although the
 * separable kernel method is far faster) but not for any transformation that
 * has any location reductions in size (as that would require low-pass
 * filtering in Fourier space, not implemented here). Pixels outside the
 * original image are taken to have the values specified by the background
 * array, which is taken to opaque black the 'background' pointer is null.
 */
#ifndef DOUBLE_IMAGE_INTERPOLATOR_H
#define DOUBLE_IMAGE_INTERPOLATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "DoubleImage.h"
#include "magic_double_image.h"

typedef struct {
  byte mksA;
  byte mksV;
  byte numChannels;
  ulong width;
  ulong height;
  ulong baseWidth;
  ulong baseHeight;
  double *_background;
  DoubleImage *_base;
} DoubleImageInterpolator;

/* Create a new interpolator for an image, with given MKS parameters a and v.
 */
void double_image_interpolator_new(
    DoubleImageInterpolator **p_this,
    DoubleImage *image,
    double *background,
    byte mks_a,
    byte mks_v);

/* Destructor.
 */
void double_image_interpolator_delete(DoubleImageInterpolator **p_this);

/* Print.
 */
void double_image_interpolator_print(DoubleImageInterpolator *this, Printer *p);

/* Interpolate.
 */
void double_image_interpolator_get(
    DoubleImageInterpolator *this,
    double x,
    double y,
    double *values);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
