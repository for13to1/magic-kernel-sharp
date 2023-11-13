#include "change_image_perspective.h"

#define DEFAULT_MKS_GENERATION_ 3
#define DEFAULT_SHARP_VERSION_ 3

int main(int argc, char *argv[]) {
  byte quality;
  char *input_filename;
  char *output_filename;
  double alpha;
  double background[4];
  DoubleImage *_din = null;
  DoubleImage *_dout = null;
  ByteImage *_in = null;
  ByteImage *_out = null;

  args_description(
      "Change the perspective of an image by shifting the direction of view");
  args_string("input-filename", "Input image to be transformed");
  args_string("output-filename", "Transformed output image to be created");
  args_real("field-of-view-degrees",
      "Field of view of the larger dimension of the input image");
  args_optional_real(
      "dov-shift-pixels-x",
      'x',
      0,
      "Number of pixels in the original image to right-shift the direction "
      "of view");
  args_optional_real(
      "dov-shift-pixels-y",
      'y',
      0,
      "Number of pixels in the original image to down-shift the direction "
      "of view");
  args_optional_real(
      "rotation-degrees",
      'd',
      0,
      "Rotate the perspective-changed image by this many degrees clockwise "
      "around its center");
  args_optional_real(
      "scale-factor",
      'k',
      1,
      "Scale the perspective-changed image by this factor around its center: "
      "k > 1 for enlargement");
  args_optional_real(
      "x-shift",
      'X',
      0,
      "Shift the final image right by this number of pixels; only the "
      "fractional part has any effect");
  args_optional_real(
      "y-shift",
      'Y',
      0,
      "Shift the final image down by this number of pixels; only the "
      "fractional part has any effect");
  args_optional_integer(
      "background-red",
      'r',
      0,
      "Red value (0-255) of the background outside the image");
  args_optional_integer(
      "background-green",
      'g',
      0,
      "Green value (0-255) of the background outside the image");
  args_optional_integer(
      "background-blue",
      'b',
      0,
      "Blue value (0-255) of the background outside the image");
  args_optional_integer(
      "background-alpha",
      'a',
      255,
      "Alpha value (0-255) of the background outside the image");
  args_optional_integer(
      "mks-generation",
      'm',
      DEFAULT_MKS_GENERATION_,
      "The generation of Magic Kernel Sharp to use");
  args_optional_integer(
      "mks-sharp-version",
      's',
      DEFAULT_SHARP_VERSION_,
      "The version 'v' of the Sharp filter to use");
  args_optional_integer(
      "margin",
      'n',
      0,
      "Number of pixels of extra margin to add around the edges of the "
      "transformed image");
  args_flag("png", 'p', "Save the output file in PNG format");
  args_flag("jpeg", 'j', "Save the output file in JPEG format");
  args_optional_integer(
      "jpeg-quality",
      'q',
      jpeg_file_get_default_quality(),
      "The JPEG quality to use for the output JPEG file, between 0 and 100");
  args_parse(argc, argv);

  input_filename = args_get_string("input-filename");

  background[3] = alpha
      = args_get_bounded_integer("background-alpha", 0, 255) / 255.0;
  background[0] = alpha * srgb_byte_gamma_to_linear(
      args_get_bounded_integer("background-red", 0, 255));
  background[1] = alpha * srgb_byte_gamma_to_linear(
      args_get_bounded_integer("background-green", 0, 255));
  background[2] = alpha * srgb_byte_gamma_to_linear(
      args_get_bounded_integer("background-blue", 0, 255));

  byte_image_new_load_file(&_in, input_filename);
  double_image_new_from_byte(&_din, _in);
  byte_image_delete(&_in);

  double_image_new_change_perspective(
      &_dout,
      _din,
      args_get_real("field-of-view-degrees"),
      args_get_real("dov-shift-pixels-x"),
      args_get_real("dov-shift-pixels-y"),
      args_get_real("rotation-degrees"),
      args_get_nonnegative_real("scale-factor"),
      args_get_real("x-shift"),
      args_get_real("y-shift"),
      background,
      args_get_nonnegative_integer("margin"),
      args_get_bounded_integer("mks-generation", 1, 6),
      args_get_bounded_integer("mks-sharp-version", 0, 7));
  double_image_delete(&_din);

  byte_image_new_from_double(&_out, _dout);
  double_image_delete(&_dout);

  output_filename = args_get_string("output-filename");
  quality = args_get_bounded_integer("jpeg-quality", 0, 100);

  if (args_get_flag_veto("png", "jpeg")) {
    byte_image_save_png(_out, output_filename);
  } else if (args_get_flag("jpeg")) {
    byte_image_save_jpeg(_out, output_filename, quality);
  } else {
    byte_image_save_quality(_out, output_filename, quality);
  }

  byte_image_delete(&_out);

  return 0;
}
