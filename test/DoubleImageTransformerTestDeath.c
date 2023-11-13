#include "DoubleImageTransformerTestDeath.h"
#include <math.h>

START_DEATH

TEST(DoubleImageTransformer, Resize) {
  DoubleImageTransformer *_ = null;

  double_image_transformer_new(&_);
  EXPECT_DEATH(
      double_image_transformer_add_resize(_, 0),
      "Scale factor cannot be zero");
  double_image_transformer_delete(&_);
}

FINISH_DEATH
