#include "jpeg_file.h"
#include <jpeglib.h>

#define DEFAULT_QUALITY_ 80

typedef struct {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
} Error_;

static byte quality_ = DEFAULT_QUALITY_;
static Lock lock_ = LOCK_INITIALIZER;
static char error_message_[JMSG_LENGTH_MAX];

static void error_(j_common_ptr cinfo);


void byte_image_new_load_jpeg(ByteImage **p_this, char *filename) {
  byte_image_new_load_jpeg_colorspace(p_this, filename, false);
}

void byte_image_new_load_jpeg_ycc(ByteImage **p_this, char *filename) {
  byte_image_new_load_jpeg_colorspace(p_this, filename, true);
}

void byte_image_new_load_jpeg_colorspace(
    ByteImage **p_this,
    char *filename,
    bool ycc) {
  byte *p;
  byte *_row = null;
  char *cs;
  ulong height;
  ulong i;
  ulong j;
  ulong width;
  struct jpeg_decompress_struct cinfo;
  Error_ err;
  File *_f = null;
  ByteImagePixel pixel;
  ByteImage *this;

  if (!file_is_jpeg(filename)) {
    throw("%s is not a recognized JPEG file", filename);
  }

  file_new(&_f, filename, "r");

  cinfo.err = jpeg_std_error(&err.pub);
  err.pub.error_exit = error_;

  if (setjmp(err.setjmp_buffer) != 0) {
    jpeg_destroy_decompress(&cinfo);
    file_delete(&_f);
    throw("JPEG decompressor error: %s", error_message_);
  }

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, ((ExternalFile *)_f->_ef)->ansifile);

  jpeg_save_markers(&cinfo, JPEG_APP0 + 2, 0xFFFF);

  jpeg_read_header(&cinfo, 1);

  if (cinfo.num_components != 3) {
    jpeg_destroy_decompress(&cinfo);
    file_delete(&_f);
    throw("JPEG file %s is not a color image", filename);
  }

  if (cinfo.jpeg_color_space != JCS_YCbCr) {
    jpeg_destroy_decompress(&cinfo);
    file_delete(&_f);

    switch (cinfo.jpeg_color_space) {
      case JCS_CMYK:
        cs = "JCS_CMYK";
        break;
      case JCS_GRAYSCALE:
        cs = "JCS_GRAYSCALE";
        break;
      case JCS_RGB:
        cs = "JCS_RGB";
        break;
      case JCS_UNKNOWN:
        cs = "JCS_UNKNOWN";
        break;
      case JCS_YCbCr:
        cs = "JCS_YCbCr";
        break;
      case JCS_YCCK:
        cs = "JCS_YCCK";
        break;
      default:
        cs = "<unknown!>";
        break;
    }

    throw("JPEG file %s has colorspace %s that is not YCbCr", cs, filename);
  }

  width = (ulong)cinfo.image_width;
  height = (ulong)cinfo.image_height;

  byte_image_new(p_this, width, height);
  this = *p_this;

  this->ycc = ycc;

  new(&_row, width * 3);

  cinfo.out_color_space = ycc ? JCS_YCbCr : JCS_RGB;
  cinfo.do_fancy_upsampling = !ycc;

  pixel.a = 255;

  jpeg_start_decompress(&cinfo);

  for (j = 0; j < height; j++) {
    if (jpeg_read_scanlines(&cinfo, &_row, 1) != 1) {
      deletev(&_row);
      jpeg_destroy_decompress(&cinfo);
      file_delete(&_f);
      throw("Failed to read row %lu of JPEG file %s", j, filename);
    }

    for (i = 0, p = _row; i < width; i++) {
      pixel.r = *p++;
      pixel.g = *p++;
      pixel.b = *p++;

      byte_image_set(this, i, j, pixel);
    }
  }

  jpeg_finish_decompress(&cinfo);

  deletev(&_row);
  jpeg_destroy_decompress(&cinfo);
  file_delete(&_f);
}

void byte_image_save_jpeg(ByteImage *this, char *filename, byte quality) {
  byte *p;
  byte *_row = null;
  ulong height;
  ulong i;
  ulong j;
  ulong width;
  struct jpeg_compress_struct cinfo;
  Error_ err;
  ByteImagePixel pixel;
  File *_f = null;

  if (quality > 100) {
    throw("Quality (%u) must be no greater than 100", quality);
  }

  file_new(&_f, filename, "w");

  cinfo.err = jpeg_std_error(&err.pub);
  err.pub.error_exit = error_;

  if (setjmp(err.setjmp_buffer) != 0) {
    jpeg_destroy_compress(&cinfo);
    file_delete(&_f);
    throw("JPEG compressor error: %s", error_message_);
  }

  width = this->width;
  height = this->height;

  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, ((ExternalFile *)_f->_ef)->ansifile);

  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, (int)quality, TRUE);

  new(&_row, width * 3);

  jpeg_start_compress(&cinfo, 1);

  for (j = 0; j < height; j++) {
    for (i = 0, p = _row; i < width; i++) {
      pixel = byte_image_get(this, i, j);

      *p++ = pixel.r;
      *p++ = pixel.g;
      *p++ = pixel.b;
    }

    if (jpeg_write_scanlines(&cinfo, &_row, 1) != 1) {
      deletev(&_row);
      jpeg_destroy_compress(&cinfo);
      file_delete(&_f);
      throw("Failed to write row %lu of JPEG file %s", j, filename);
    }
  }

  jpeg_finish_compress(&cinfo);

  deletev(&_row);
  jpeg_destroy_compress(&cinfo);
  file_delete(&_f);
}

bool file_is_jpeg(char *filename) {
  struct jpeg_decompress_struct cinfo;
  Error_ err;
  File *_f = null;

  file_new(&_f, filename, "r");

  cinfo.err = jpeg_std_error(&err.pub);
  err.pub.error_exit = error_;

  if (setjmp(err.setjmp_buffer) != 0) {
    jpeg_destroy_decompress(&cinfo);
    file_delete(&_f);
    return false;
  }

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, ((ExternalFile *)_f->_ef)->ansifile);

  jpeg_read_header(&cinfo, 1);

  jpeg_destroy_decompress(&cinfo);
  file_delete(&_f);

  return true;
}

byte jpeg_file_set_default_quality(byte default_quality) {
  byte current;

  lock(&lock_);

  if (default_quality > 100) {
    unlock(&lock_);
    throw("Quality (%u) must be no greater than 100", default_quality);
  }

  current = quality_;
  quality_ = default_quality;

  unlock(&lock_);

  return current;
}

byte jpeg_file_get_default_quality(void) {
  return quality_;
}


static void error_(j_common_ptr cinfo) {
  Error_ *err;

  err = (Error_ *)cinfo->err;
  cinfo->err->format_message(cinfo, error_message_);
  longjmp(err->setjmp_buffer, 1);
}
