#include "make_gaussian_blob.h"
#include <math.h>

int main(int argc, char *argv[]) {
  byte quality;
  char *filename;
  ulong i;
  ulong j;
  ulong width;
  ulong height;
  double center_x;
  double center_y;
  double r;
  double radius;
  double x;
  double y;
  DoubleImage *_dout = null;
  ByteImage *_out = null;

  args_description("Make a Gaussian blob image.");
  args_string("filename", "Filename of image file to be created");
  args_integer("width", "Width of image");
  args_integer("height", "Height of the image");
  args_real("radius", "Radius of the blob");
  args_flag("png", 'p', "Save the output file in PNG format");
  args_flag("jpeg", 'j', "Save the output file in JPEG format");
  args_optional_integer(
      "jpeg-quality",
      'q',
      jpeg_file_get_default_quality(),
      "The JPEG quality to use for the output JPEG file, between 0 and 100");
  args_parse(argc, argv);

  width = args_get_positive_integer("width");
  height = args_get_positive_integer("height");
  radius = args_get_positive_real("radius");

  double_image_new_y(&_dout, width, height);

  center_x = ((double)width - 1) / 2;
  center_y = ((double)height - 1) / 2;

  for (i = 0; i < width; i++) {
    for (j = 0; j < height; j++) {
      x = (double)i - center_x;
      y = (double)j - center_y;
      r = sqrt(x * x + y * y);

      _dout->y[i][j] = normal(r / radius);
    }
  }

  double_image_auto_scale(_dout);

  byte_image_new_from_double(&_out, _dout);
  double_image_delete(&_dout);

  filename = args_get_string("filename");
  quality = args_get_bounded_integer("jpeg-quality", 0, 100);

  if (args_get_flag_veto("png", "jpeg")) {
    byte_image_save_png(_out, filename);
  } else if (args_get_flag("jpeg")) {
    byte_image_save_jpeg(_out, filename, quality);
  } else {
    byte_image_save_quality(_out, filename, quality);
  }

  byte_image_delete(&_out);

  return 0;
}
