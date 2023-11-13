#include "png_file.h"
#include <png.h>

void byte_image_new_load_png(ByteImage **p_this, char *filename) {
  byte bit_depth;
  byte color_type;
  byte header[8];
  ulong width;
  ulong height;
  png_structp png;
  png_infop info;
  png_infop end;
  File *_f = null;
  FILE *ansifile;
  ByteImage *this;
  int srgb;

  file_new(&_f, filename, "r");
  ansifile = ((ExternalFile *)_f->_ef)->ansifile;

  file_read_bytes(_f, header, 8);

  if (png_sig_cmp(header, 0, 8) != 0) {
    throw("File '%s' not recognized as a PNG file", filename);
  }

  if ((png = png_create_read_struct(
          PNG_LIBPNG_VER_STRING,
          null,
          null,
          null)) == null) {
    throw("Failed to create PNG file");
  }

  if ((info = png_create_info_struct(png)) == null) {
    throw("Failed to create info");
  }

  if ((end = png_create_info_struct(png)) == null) {
    throw("Failed to create end");
  }

  if (setjmp(png_jmpbuf(png)) != 0) {
    throw("Failed to read header");
  }

  png_init_io(png, ansifile);
  png_set_sig_bytes(png, 8);

  png_read_info(png, info);

  if (png_get_sRGB(png, info, &srgb) != 0
      && srgb != PNG_sRGB_INTENT_PERCEPTUAL) {
    throw("Can only read perceptual sRGB images; intent was %d", srgb);
  }

  width = png_get_image_width(png, info);
  height = png_get_image_height(png, info);

  bit_depth = png_get_bit_depth(png, info);
  color_type = png_get_color_type(png, info);
  png_set_expand(png);

  if (bit_depth == 16) {
    png_set_strip_16(png);
  }

  if (bit_depth < 8) {
    png_set_packing(png);
  }

  if (color_type == PNG_COLOR_TYPE_GRAY
      || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(png);
  }

  if (color_type == PNG_COLOR_TYPE_RGB
      || color_type == PNG_COLOR_TYPE_GRAY
      || color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_filler(png, 0xff, PNG_FILLER_AFTER);
  }

  byte_image_new(p_this, width, height);
  this = *p_this;

  png_set_interlace_handling(png);
  png_read_update_info(png, info);

  if (setjmp(png_jmpbuf(png)) != 0) {
    throw("Failed to read image");
  }

  png_read_image(png, (byte **)this->_pixels);
  png_read_end(png, end);
  png_destroy_read_struct(&png, &info, &end);

  file_delete(&_f);
}

void byte_image_save_png(ByteImage *this, char *filename) {
  ulong width;
  ulong height;
  png_structp png;
  png_infop info;
  File *_f = null;

  width = this->width;
  height = this->height;

  file_new(&_f, filename, "w");

  if ((png = png_create_write_struct(
          PNG_LIBPNG_VER_STRING,
          null,
          null,
          null)) == null) {
    throw("Failed to create PNG file");
  }

  if ((info = png_create_info_struct(png)) == null) {
    throw("Failed to create info");
  }

  if (setjmp(png_jmpbuf(png)) != 0) {
    throw("Error in initialization");
  }

  png_init_io(png, ((ExternalFile *)_f->_ef)->ansifile);

  if (setjmp(png_jmpbuf(png)) != 0) {
    throw("Failed to write header");
  }

  png_set_IHDR(
      png,
      info,
      width,
      height,
      8,
      6,
      PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_BASE,
      PNG_FILTER_TYPE_BASE);

  png_set_sRGB(png, info, PNG_sRGB_INTENT_PERCEPTUAL);
  png_write_info(png, info);

  if (setjmp(png_jmpbuf(png)) != 0) {
    throw("Failed to write image");
  }

  png_write_image(png, (byte **)this->_pixels);

  if (setjmp(png_jmpbuf(png)) != 0) {
    throw("Failed to end writing of image");
  }

  png_write_end(png, null);
  png_destroy_write_struct(&png, &info);
  file_delete(&_f);
}

bool file_is_png(char *filename) {
  bool is_png;
  byte header[8];
  File *_f = null;

  file_new(&_f, filename, "r");

  file_read_bytes(_f, header, 8);
  is_png = png_sig_cmp(header, 0, 8) == 0;

  file_delete(&_f);

  return is_png;
}

void png_file_get_dimensions(char *filename, ulong *width, ulong *height) {
  byte header[8];
  png_structp png;
  png_infop info;
  png_infop end;
  File *_f = null;
  FILE *ansifile;

  file_new(&_f, filename, "r");
  ansifile = ((ExternalFile *)_f->_ef)->ansifile;

  file_read_bytes(_f, header, 8);

  if (png_sig_cmp(header, 0, 8) != 0) {
    throw("File '%s' not recognized as a PNG file", filename);
  }

  if ((png = png_create_read_struct(
          PNG_LIBPNG_VER_STRING,
          null,
          null,
          null)) == null) {
    throw("Failed to create PNG file");
  }

  if ((info = png_create_info_struct(png)) == null) {
    throw("Failed to create info");
  }

  if ((end = png_create_info_struct(png)) == null) {
    throw("Failed to create end");
  }

  if (setjmp(png_jmpbuf(png)) != 0) {
    throw("Failed to read header");
  }

  png_init_io(png, ansifile);
  png_set_sig_bytes(png, 8);

  png_read_info(png, info);

  *width = png_get_image_width(png, info);
  *height = png_get_image_height(png, info);

  png_destroy_read_struct(&png, &info, &end);

  file_delete(&_f);
}
