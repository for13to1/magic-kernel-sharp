/* Static library providing methods for applying Magic Kernel Sharp and related
 * kernels to DoubleImages, as efficiently as possible.
 */
#ifndef MAGIC_DOUBLE_IMAGE_H
#define MAGIC_DOUBLE_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "double_image_kernel.h"
#include "magic.h"

/* Resize a DoubleImage. The methods available, and the parameters 'a' and 'v,'
 * are described in magic.h.
 */
void double_image_new_resize(
    DoubleImage **p_this,
    DoubleImage *other,
    ulong width,
    ulong height,
    double k,
    char *method,
    byte a,
    byte v);

/* Blur a DoubleImage. The "width" (two standard deviations) of the blurring is
 * approximately equal to 'blur'.
 */
void double_image_new_blur(
    DoubleImage **p_this,
    DoubleImage *other,
    double blur);

/* Sharpen an image using the simplest sharpening kernel, which is just v = 1
 * of the Magic Sharp kernel. A sharpening of 1.0 "undoes" a blur of 1.0 above,
 * to first order.
 */
void double_image_new_sharpen(
    DoubleImage **p_this,
    DoubleImage *other,
    double sharpening);

/* Apply the Magic Sharp kernel to an image.
 */
void double_image_new_magic_sharpen(
    DoubleImage **p_this,
    DoubleImage *other,
    byte a,
    byte v);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
