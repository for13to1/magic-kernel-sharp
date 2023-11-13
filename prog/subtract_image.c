#include "subtract_image.h"

int main(int argc, char *argv[]) {
  byte quality;
  char *filename;
  ulong i;
  ulong j;
  ulong width;
  ulong height;
  double m;
  ByteImagePixel p1;
  ByteImagePixel p2;
  ByteImagePixel pdiff;
  ByteImage *_out = null;
  ByteImage *_in1 = null;
  ByteImage *_in2 = null;

  args_description(
      "Subtract an image from another of the same size, representing the "
      "difference as an image, where mid-gray represents a zero difference.");
  args_string(
      "input-filename-1",
      "Image to be subtracted from");
  args_string(
      "input-filename-2",
      "Image to be subtracted");
  args_string("output-filename", "Difference image to be created");
  args_optional_real(
      "magnify",
      'm',
      1,
      "The difference from mid-gray in the output will be $ units of "
      "intensity for each unit of intensity difference of the input files");
  args_flag("png", 'p', "Save the output file in PNG format");
  args_flag("jpeg", 'j', "Save the output file in JPEG format");
  args_optional_integer(
      "jpeg-quality",
      'q',
      jpeg_file_get_default_quality(),
      "The JPEG quality to use for the output JPEG file, between 0 and 100");
  args_parse(argc, argv);

  byte_image_new_load_file(&_in1, args_get_string("input-filename-1"));
  byte_image_new_load_file(&_in2, args_get_string("input-filename-2"));

  if ((width = _in1->width) != _in2->width) {
    throw("Images have different widths (%lu v %lu)", width, _in2->width);
  }

  if ((height = _in1->height) != _in2->height) {
    throw("Images have different heights (%lu v %lu)", height, _in2-height);
  }

  if (byte_image_contains_transparency(_in1)
      || byte_image_contains_transparency(_in2)) {
    throw("Cannot use this program on images with transparency");
  }

  m = args_get_positive_real("magnify");

  byte_image_new(&_out, width, height);

  pdiff.a = 255;

  for (j = 0; j < height; j++) {
    for (i = 0; i < width; i++) {
      p1 = byte_image_get(_in1, i, j);
      p2 = byte_image_get(_in2, i, j);

      pdiff.r = byte_encode_diff(
          bound(m * ((double)p1.r - (double)p2.r) / 255, -1, 1));
      pdiff.g = byte_encode_diff(
          bound(m * ((double)p1.g - (double)p2.g) / 255, -1, 1));
      pdiff.b = byte_encode_diff(
          bound(m * ((double)p1.b - (double)p2.b) / 255, -1, 1));
      byte_image_set(_out, i, j, pdiff);
    }
  }

  byte_image_delete(&_in2);
  byte_image_delete(&_in1);

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
