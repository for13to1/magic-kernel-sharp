#include "byte_image_file.h"

void byte_image_new_load_file(ByteImage **p_this, char *filename) {
  char *filetype;

  filetype = byte_image_filetype(filename);

  if (filetype == null) {
    throw("%s cannot be recognized as a PNG or JPEG file", filename);
  }

  if (string_equals(filetype, BYTE_IMAGE_FILETYPE_PNG)) {
    byte_image_new_load_png(p_this, filename);

    return;
  }

  if (string_equals(filetype, BYTE_IMAGE_FILETYPE_JPEG)) {
    byte_image_new_load_jpeg(p_this, filename);

    return;
  }

  throw("Impossible");
}

void byte_image_save_file(ByteImage *this, char *filename) {
  byte_image_save_quality(this, filename, jpeg_file_get_default_quality());
}

void byte_image_save_quality(ByteImage *this, char *filename, byte quality) {
  char *filetype;

  filetype = byte_image_filetype_from_filename(filename);

  if (filetype == null) {
    throw("Could not infer the filetype desired from filename '%s'", filename);
  }

  if (string_equals(filetype, BYTE_IMAGE_FILETYPE_PNG)) {
    byte_image_save_png(this, filename);

    return;
  }

  if (string_equals(filetype, BYTE_IMAGE_FILETYPE_JPEG)) {
    byte_image_save_jpeg(this, filename, quality);

    return;
  }

  throw("impossible");
}

void byte_image_save_type_quality(
    ByteImage *this,
    char *filename,
    char *filetype,
    byte quality) {
  if (filetype == null) {
    byte_image_save_quality(this, filename, quality);
  } else if (string_equals(filetype, BYTE_IMAGE_FILETYPE_PNG)) {
    byte_image_save_png(this, filename);
  } else if (string_equals(filetype, BYTE_IMAGE_FILETYPE_JPEG)) {
    byte_image_save_jpeg(this, filename, quality);
  } else {
    throw("Filetype '%s' not recognized", filetype);
  }
}

void double_image_save_file(
    DoubleImage *this,
    bool auto_scale,
    char *fmt,
    ...) {
  char *_filename = null;
  ByteImage *_bi = null;
  DoubleImage *_di = null;
  va_list ap1;
  va_list ap2;

  va_start(ap1, fmt);
  va_start(ap2, fmt);
  string_new_vf(&_filename, fmt, ap1, ap2);
  va_end(ap1);
  va_end(ap2);

  double_image_new_copy(&_di, this);

  if (auto_scale) {
    double_image_auto_scale(_di);
  }

  if (_di->gamma) {
    double_image_gamma_to_linear(_di);
  }

  byte_image_new_from_double(&_bi, auto_scale ? _di : this);
  double_image_delete(&_di);

  byte_image_save_file(_bi, _filename);
  byte_image_delete(&_bi);
  string_delete(&_filename);
}

char *byte_image_filetype(char *filename) {
  if (file_is_png(filename)) {
    return BYTE_IMAGE_FILETYPE_PNG;
  }

  if (file_is_jpeg(filename)) {
    return BYTE_IMAGE_FILETYPE_JPEG;
  }

  return null;
}

char *byte_image_filetype_from_filename(char *filename) {
  if (string_ends_with(filename, ".png", true)) {
    return BYTE_IMAGE_FILETYPE_PNG;
  }

  if (string_ends_with(filename, ".jpg", true)
      || string_ends_with(filename, ".jpeg", true)
      || string_ends_with(filename, ".jpe", true)
      || string_ends_with(filename, ".jif", true)
      || string_ends_with(filename, ".jfif", true)
      || string_ends_with(filename, ".jfi", true)) {
    return BYTE_IMAGE_FILETYPE_JPEG;
  }

  return null;
}
