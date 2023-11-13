#include "blur_image.h"

int main(int argc, char *argv[]) {
  byte quality;
  char *filename;
  ByteImage *_in = null;
  ByteImage *_out = null;
  DoubleImage *_din = null;
  DoubleImage *_dout = null;

  args_description("Blur an image symmetrically using the Magic Kernel");
  args_string("input-filename", "Input image to be blurred");
  args_real("blur", "The amount of blur to be applied");
  args_string("output-filename", "Blurred output image to be created");
  args_flag("auto-scale", 's', "Auto-scale the output intensity range");
  args_flag("png", 'p', "Save the output file in PNG format");
  args_flag("jpeg", 'j', "Save the output file in JPEG format");
  args_optional_integer(
      "jpeg-quality",
      'q',
      jpeg_file_get_default_quality(),
      "The JPEG quality to use for the output JPEG file, between 0 and 100");
  args_parse(argc, argv);

  byte_image_new_load_file(&_in, args_get_string("input-filename"));
  double_image_new_from_byte(&_din, _in);
  byte_image_delete(&_in);
  double_image_new_blur(
      &_dout,
      _din,
      args_get_positive_real("blur"));
  double_image_delete(&_din);

  if (args_set("auto-scale")) {
    double_image_auto_scale(_dout);
  }

  byte_image_new_from_double(&_out, _dout);
  double_image_delete(&_dout);

  filename = args_get_string("output-filename");
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
