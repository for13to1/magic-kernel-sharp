#include "resize_image.h"
#include <math.h>

int main(int argc, char *argv[]) {
  bool no_gamma_handling;
  byte quality;
  char *filename;
  char *_s = null;
  ulong in_width;
  ulong in_height;
  ulong out_width;
  ulong out_height;
  ulong m;
  double extra_sharp;
  double k;
  double k_recip;
  ByteImage *_in = null;
  ByteImage *_out = null;
  DoubleImage *_din = null;
  DoubleImage *_dtemp = null;
  DoubleImage *_dout = null;

  args_description("Resize an image file");
  args_string("input-filename", "Input image to be resized");
  args_string("output-filename", "Resized output image to be created");
  args_optional_integer(
      "width",
      'w',
      ARGS_INTEGER_NO_DEFAULT,
      "The width of the output image, in pixels");
  args_optional_integer(
      "height",
      'H',
      ARGS_INTEGER_NO_DEFAULT,
      "The height of the output image, in pixels");
  args_optional_real(
      "scale-factor",
      'k',
      ARGS_REAL_NO_DEFAULT,
      "The scale factor k, where k > 1 for upsizing");
  args_optional_real(
      "recip-scale-factor",
      'r',
      ARGS_REAL_NO_DEFAULT,
      "The reciprocal 1/k of the scale factor k; useful for specifying "
      "exact integer values of 1/k");
  string_new_copy(&_s, "The resize method, which must be one of:");

  for (m = 0; m < MAGIC_NUM_RESIZERS; m++) {
    string_renew_append_f(
        &_s,
        "\n\t%s: %s",
        MAGIC_RESIZERS[2 * m],
        MAGIC_RESIZERS[2 * m + 1]);
  }

  args_optional_string("method", 'm', MAGIC_RESIZER_MAGIC_KERNEL_SHARP, _s);
  string_delete(&_s);
  args_optional_integer(
      "a",
      'a',
      3,
      "The generation 'a' of Magic Kernel Sharp, the Magic Kernel, or Lanczos "
      "kernel, as described in the paper above (valid values for each are "
      "listed under METHOD above)");
  args_optional_integer(
      "v",
      'v',
      3,
      "The version of approximation 'v' to the exact Sharp filter in "
      "Magic Kernel Sharp (valid values are listed above)");
  args_optional_real(
      "extra-sharpening",
      's',
      0,
      "Extra sharpening to be applied to the image");
  args_flag("auto-scale", 'S', "Auto-scale the output intensity range");
  args_flag("no-gamma-handling",
      'g',
      "Do not handle gamma encoding properly: transform in gamma-encoded "
      "space rather than linear space");
  args_flag("png", 'p', "Save the output file in PNG format");
  args_flag("jpeg", 'j', "Save the output file in JPEG format");
  args_optional_integer(
      "jpeg-quality",
      'q',
      jpeg_file_get_default_quality(),
      "The JPEG quality to use for the output JPEG file, between 0 and 100");
  args_parse(argc, argv);

  filename = args_get_string("input-filename");

  byte_image_new_load_file(&_in, args_get_string("input-filename"));
  double_image_new_from_byte(&_din, _in);
  byte_image_delete(&_in);

  no_gamma_handling = args_set("no-gamma-handling");

  if (no_gamma_handling) {
    double_image_linear_to_gamma(_din);
  }

  in_width = _din->width;
  in_height = _din->height;

  if (args_set_veto("scale-factor", "recip-scale-factor")) {
    k = args_get_positive_real("scale-factor");

    if (args_set("width")) {
      out_width = args_get_positive_integer("width");
    } else {
      out_width = (ulong)ceil(k * in_width);
    }

    if (args_set("height")) {
      out_height = args_get_positive_integer("height");
    } else {
      out_height = (ulong)ceil(k * in_height);
    }
  } else if (args_set("recip-scale-factor")) {
    k = 1 / (k_recip = args_get_positive_real("recip-scale-factor"));

    if (args_set("width")) {
      out_width = args_get_positive_integer("width");
    } else {
      out_width = (ulong)ceil(in_width / k_recip);
    }

    if (args_set("height")) {
      out_height = args_get_positive_integer("height");
    } else {
      out_height = (ulong)ceil(in_height / k_recip);
    }
  } else if (args_set("width")) {
    out_width = args_get_positive_integer("width");

    if (args_set("height")) {
      out_height = args_get_positive_integer("height");
      k = ((double)out_width / in_width + (double)out_height / in_height) / 2;
    } else {
      k = (double)out_width / in_width;
      out_height = (ulong)ceil(k * in_height);
    }
  } else if (args_set("height")) {
    out_height = args_get_positive_integer("height");
    k = (double)out_height / in_height;
    out_width = (ulong)ceil(k * in_width);
  } else {
    throw("Must specify at least one of --scale-factor, "
        "--recip-scale-factor, --width, or --height");
    k = 0;
    out_width = 0;
    out_height = 0;
  }

  extra_sharp = args_get_bounded_real("extra-sharpening", -1, DBL_MAX);

  if (k >= 1 && extra_sharp != 0) {
    double_image_new_sharpen(
        &_dtemp,
        _din,
        extra_sharp);
    double_image_delete(&_din);
    donate(&_dtemp, &_din);
  }

  double_image_new_resize(
      &_dout,
      _din,
      out_width,
      out_height,
      k,
      args_get_string("method"),
      (byte)args_get_bounded_integer("a", 1, 6),
      (byte)args_get_bounded_integer("v", 1, 7));

  double_image_delete(&_din);

  if (k < 1 && extra_sharp != 0) {
    double_image_new_sharpen(
        &_dtemp,
        _dout,
        extra_sharp);
    double_image_delete(&_dout);
    donate(&_dtemp, &_dout);
  }

  if (args_set("auto-scale")) {
    double_image_auto_scale(_dout);
  }

  if (no_gamma_handling) {
    double_image_gamma_to_linear(_dout);
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
