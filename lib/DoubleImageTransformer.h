/* Class for transforming coordinates of a DoubleImage. The transforms are
 * applied in order from in to out, and in reverse order from out to in.
 * Note that pointers to simple parameters are passed in rather than values,
 * since passthrough pointers ultimately need to be passed, and we don't want
 * to allocate extra memory inside the transformer to store these values.
 */
#ifndef DOUBLE_IMAGE_TRANSFORMER_H
#define DOUBLE_IMAGE_TRANSFORMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jpc.h"

typedef struct {
  ulong n;
  void (**_inToOuts)(double *x, double *y, void *p);
  void (**_outToIns)(double *x, double *y, void *p);
  void **_ps;
} DoubleImageTransformer;

typedef struct {
  double xShift;
  double yShift;
} DoubleImageTransformerShiftInfo;

typedef struct {
  double cSquared;
  double x0;
} DoubleImageTransformerPerspectiveInfo;

/* Create a new, empty transformer.
 */
void double_image_transformer_new(DoubleImageTransformer **p_this);

/* Destructor.
 */
void double_image_transformer_delete(DoubleImageTransformer **p_this);

/* Add a general transform, specifying the functions for both in to out and out
 * to in, as well as any passthrough object.
 */
void double_image_transformer_add(
    DoubleImageTransformer *this,
    void (*in_to_out)(double *x, double *y, void *p),
    void (*out_to_in)(double *x, double *y, void *p),
    void *p);

/* Add a rotation around the origin.
 */
void double_image_transformer_add_rotation(
    DoubleImageTransformer *this,
    double *degrees_clockwise);

/* Add a resize around the origin.
 */
void double_image_transformer_add_resize(
    DoubleImageTransformer *this,
    double *scale_factor);

/* Add a shift.
 */
void double_image_transformer_add_shift(
    DoubleImageTransformer *this,
    DoubleImageTransformerShiftInfo *info);

/* Add a perspective change. See double_image_new_change_perspective() in
 * double_image_transform.c for how those two parameters are computed.
 */
void double_image_transformer_add_perspective_change(
    DoubleImageTransformer *this,
    DoubleImageTransformerPerspectiveInfo *info);

/* Transform from in coordinates to out coordinates, in place.
 */
void double_image_transformer_in_to_out(
    DoubleImageTransformer *this,
    double *x,
    double *y);

/* Transform from out coordinates to in coordinates, in place.
 */
void double_image_transformer_out_to_in(
    DoubleImageTransformer *this,
    double *x,
    double *y);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
