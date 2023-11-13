#include "TextFileTest.h"
#include <ctype.h>
#include <float.h>

#define N_ 20
#define MAX_RECORDS_WIDTH_ 16

typedef struct {
  ulong numFields;
  ulong *_fieldLengths;
  char **_fields;
} Record_;

static void record_delete_(Record_ **p_this);


START

TEST(TextFile, New) {
  char *_s = null;
  TempFilename *_tfn = null;
  TextFile *_tf = null;

  temp_filename_new(&_tfn, "csv");
  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_STREQ(_tfn->_filename, text_file_get_filename(_tf));
  EXPECT_ULEQ(',', _tf->separator);
  text_file_write(_tf, "dog");
  text_file_write(_tf, "cat");
  text_file_end_record(_tf);
  text_file_end_record(_tf);
  text_file_write(_tf, "woof");
  text_file_end_record(_tf);
  text_file_delete(&_tf);
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ("dog,cat\n\nwoof\n", _s);
  string_delete(&_s);
  temp_filename_delete(&_tfn);

  temp_filename_new(&_tfn, "tsv");
  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_ULEQ('\t', _tf->separator);
  text_file_write(_tf, "dog");
  text_file_write(_tf, "cat");
  text_file_end_record(_tf);
  text_file_delete(&_tf);
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ("dog\tcat\n", _s);
  string_delete(&_s);
  temp_filename_delete(&_tfn);

  temp_filename_new(&_tfn, "hsv");
  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_ULEQ('\001', _tf->separator);
  text_file_write(_tf, "dog");
  text_file_write(_tf, "cat");
  text_file_end_record(_tf);
  text_file_delete(&_tf);
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ("dog\001cat\n", _s);
  string_delete(&_s);
  temp_filename_delete(&_tfn);

  temp_filename_new(&_tfn, "nsv");
  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_ULEQ('\0', _tf->separator);
  text_file_write(_tf, "dog");
  text_file_end_record(_tf);
  text_file_delete(&_tf);
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ("dog\n", _s);
  string_delete(&_s);
  temp_filename_delete(&_tfn);

  temp_filename_new(&_tfn, "txt");
  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  EXPECT_ULEQ('\0', _tf->separator);
  text_file_write(_tf, "dog");
  text_file_end_record(_tf);
  text_file_delete(&_tf);
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ("dog\n", _s);
  string_delete(&_s);
  temp_filename_delete(&_tfn);
}

TEST(TextFile, NewType) {
  char *_s = null;
  TempFilename *_tfn = null;
  TextFile *_tf = null;

  text_file_new_type(&_tf, null, null, "csv");
  EXPECT_TRUE(file_is_memory(_tf->_file));
  EXPECT_ULEQ(',', _tf->separator);
  text_file_write(_tf, "dog");
  text_file_write(_tf, "cat");
  text_file_end_record(_tf);
  text_file_end_record(_tf);
  text_file_write(_tf, "woof");
  text_file_end_record(_tf);
  string_new_memory_file_contents(&_s, _tf->_file->_mf);
  text_file_delete(&_tf);
  EXPECT_STREQ("dog,cat\n\nwoof\n", _s);
  string_delete(&_s);

  text_file_new_type(&_tf, null, "stdin", "csv");
  EXPECT_PEQ(stdin, ((ExternalFile *)_tf->_file->_ef)->ansifile);
  text_file_delete(&_tf);

  text_file_new_type(&_tf, null, "stdout", "csv");
  EXPECT_PEQ(stdout, ((ExternalFile *)_tf->_file->_ef)->ansifile);
  text_file_delete(&_tf);

  text_file_new_type(&_tf, null, "stderr", "csv");
  EXPECT_PEQ(stderr, ((ExternalFile *)_tf->_file->_ef)->ansifile);
  text_file_delete(&_tf);

  text_file_new_type(&_tf, "-", "r", "csv");
  EXPECT_PEQ(stdin, ((ExternalFile *)_tf->_file->_ef)->ansifile);
  text_file_delete(&_tf);

  text_file_new_type(&_tf, "-", "w", "csv");
  EXPECT_PEQ(stdout, ((ExternalFile *)_tf->_file->_ef)->ansifile);
  text_file_delete(&_tf);

  temp_filename_new(&_tfn, null);

  text_file_new_type(&_tf, _tfn->_filename, "w", "csv");
  temp_filename_unlock(_tfn);
  EXPECT_ULEQ(',', _tf->separator);
  text_file_delete(&_tf);

  text_file_new_type(&_tf, _tfn->_filename, "w", "tsv");
  EXPECT_ULEQ('\t', _tf->separator);
  text_file_delete(&_tf);

  text_file_new_type(&_tf, _tfn->_filename, "w", "hsv");
  EXPECT_ULEQ('\001', _tf->separator);
  text_file_delete(&_tf);

  text_file_new_type(&_tf, _tfn->_filename, "w", "nsv");
  EXPECT_ULEQ('\0', _tf->separator);
  text_file_delete(&_tf);

  text_file_new_type(&_tf, _tfn->_filename, "w", "txt");
  EXPECT_ULEQ('\0', _tf->separator);
  text_file_delete(&_tf);

  temp_filename_delete(&_tfn);
}

TEST(TextFile, NewMemory) {
  char *_s = null;
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  EXPECT_ULEQ(',', _tf->separator);
  text_file_write(_tf, "dog");
  text_file_write(_tf, "cat");
  text_file_end_record(_tf);
  text_file_end_record(_tf);
  text_file_write(_tf, "woof");
  text_file_end_record(_tf);
  string_new_memory_file_contents(&_s, _tf->_file->_mf);
  text_file_delete(&_tf);
  EXPECT_STREQ("dog,cat\n\nwoof\n", _s);
  string_delete(&_s);

  text_file_new_memory(&_tf, "tsv");
  EXPECT_ULEQ('\t', _tf->separator);
  text_file_write(_tf, "dog");
  text_file_write(_tf, "cat");
  text_file_end_record(_tf);
  string_new_memory_file_contents(&_s, _tf->_file->_mf);
  text_file_delete(&_tf);
  EXPECT_STREQ("dog\tcat\n", _s);
  string_delete(&_s);

  text_file_new_memory(&_tf, "hsv");
  EXPECT_ULEQ('\001', _tf->separator);
  text_file_write(_tf, "dog");
  text_file_write(_tf, "cat");
  text_file_end_record(_tf);
  string_new_memory_file_contents(&_s, _tf->_file->_mf);
  text_file_delete(&_tf);
  EXPECT_STREQ("dog\001cat\n", _s);
  string_delete(&_s);

  text_file_new_memory(&_tf, "nsv");
  EXPECT_ULEQ('\0', _tf->separator);
  text_file_write(_tf, "dog");
  text_file_end_record(_tf);
  string_new_memory_file_contents(&_s, _tf->_file->_mf);
  text_file_delete(&_tf);
  EXPECT_STREQ("dog\n", _s);
  string_delete(&_s);

  text_file_new_memory(&_tf, "txt");
  EXPECT_ULEQ('\0', _tf->separator);
  text_file_write(_tf, "dog");
  text_file_end_record(_tf);
  string_new_memory_file_contents(&_s, _tf->_file->_mf);
  text_file_delete(&_tf);
  EXPECT_STREQ("dog\n", _s);
  string_delete(&_s);
}

TEST(TextFile, TryNew) {
  TempFilename *_tfn = null;
  TextFile *_tf = null;

  temp_filename_new(&_tfn, "csv");
  EXPECT_FALSE(text_file_try_new(&_tf, _tfn->_filename, "r"));
  EXPECT_TRUE(text_file_try_new(&_tf, _tfn->_filename, "w"));
  temp_filename_unlock(_tfn);
  text_file_delete(&_tf);
  temp_filename_delete(&_tfn);
}

TEST(TextFile, TryNewType) {
  TempFilename *_tfn = null;
  TextFile *_tf = null;

  temp_filename_new(&_tfn, "csv");
  EXPECT_FALSE(text_file_try_new_type(&_tf, _tfn->_filename, "r", "csv"));
  EXPECT_TRUE(text_file_try_new_type(&_tf, _tfn->_filename, "w", "csv"));
  temp_filename_unlock(_tfn);
  text_file_delete(&_tf);
  temp_filename_delete(&_tfn);

  EXPECT_TRUE(text_file_try_new_type(&_tf, null, null, "csv"));
  text_file_delete(&_tf);

  EXPECT_TRUE(text_file_try_new_type(&_tf, "-", "r", "csv"));
  text_file_delete(&_tf);
}

TEST(TextFile, Print) {
  TextFile *_ = null;

  text_file_new_memory(&_, "csv");
  text_file_write(_, "dog");
  text_file_write(_, "cat");
  text_file_end_record(_);
  text_file_delete(&_);
}

TEST(TextFile, WriteRead) {
  byte field_length_width;
  byte num_fields_width;
  byte num_records_width;
  byte newline_type;
  char c;
  char *p;
  char *pe;
  char **fields;
  ulong f;
  ulong field_length;
  ulong num_fields;
  ulong num_records;
  ulong u;
  ulong v;
  TextFile *_tf = null;
  Random *_random = null;
  Record_ *record;
  Record_ **_records = null;

  random_new(&_random);

  for (v = 0; v < N_; v++) {
    newline_type = random_modulo(_random, 4);
    text_file_new_memory(&_tf, "csv");
    num_records_width = random_modulo(_random, MAX_RECORDS_WIDTH_);
    num_records = random_modulo(_random, 1 << num_records_width);
    new(&_records, num_records);

    for (u = 0; u < num_records; u++) {
      newo(&_records[u]);
      record = _records[u];

      num_fields_width = random_modulo(_random, 9);
      record->numFields = random_modulo(_random, 1 << num_fields_width);
      new(&record->_fieldLengths, record->numFields);
      new(&record->_fields, record->numFields);

      for (f = 0; f < record->numFields; f++) {
        field_length_width = random_modulo(_random, 5);
        field_length = random_modulo(_random, 1 << field_length_width);
        record->_fieldLengths[f] = field_length;
        new(&record->_fields[f], field_length + 1);

        for (p = record->_fields[f], pe = p + field_length; p < pe; p++) {
          do {
            c = random_modulo(_random, 127);
          } while (c == ',' || !isprint(c));

          *p = c;
        }

        *p = '\0';
        text_file_write(_tf, record->_fields[f]);
      }

      switch (newline_type) {
        case 0:
          text_file_end_record(_tf);
          break;
        case 1:
          file_write_byte(_tf->_file, '\n');
          _tf->currentFieldIndex = 0;
          break;
        case 2:
          file_write_byte(_tf->_file, '\r');
          _tf->currentFieldIndex = 0;
          break;
        default:
          file_write_byte(_tf->_file, '\r');
          file_write_byte(_tf->_file, '\n');
          _tf->currentFieldIndex = 0;
          break;
      }

      if (record->numFields == 1 && record->_fields[0][0] == '\0') {
        record->numFields = 0;
        deletev(&record->_fields[0]);
      }
    }

    text_file_rewind(_tf);

    for (u = 0; u < num_records; u++) {
      record = _records[u];
      num_fields = text_file_read(_tf, &fields);
      EXPECT_ULEQ(record->numFields, num_fields);

      for (f = 0; f < num_fields; f++) {
        EXPECT_STREQ(record->_fields[f], fields[f]);
      }
    }

    delete(&_records, num_records, record_delete_);
    text_file_delete(&_tf);
  }

  random_delete(&_random);
}

TEST(TextFile, WriteF) {
  char *_s = null;
  TempFilename *_tfn = null;
  TextFile *_tf = null;

  temp_filename_new(&_tfn, "csv");
  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  text_file_write_f(_tf, "dog");
  text_file_write_f(_tf, "c%s", "at");
  text_file_end_record(_tf);
  text_file_end_record(_tf);
  text_file_write_f(_tf, "woof-%lu%c", 123, '!');
  text_file_end_record(_tf);
  text_file_delete(&_tf);
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ("dog,cat\n\nwoof-123!\n", _s);
  string_delete(&_s);
  temp_filename_delete(&_tfn);
}

TEST(TextFile, WriteBool) {
  char *_s = null;
  TempFilename *_tfn = null;
  TextFile *_tf = null;

  temp_filename_new(&_tfn, "csv");
  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  text_file_write_bool(_tf, true);
  text_file_write_bool(_tf, false);
  text_file_end_record(_tf);
  text_file_end_record(_tf);
  text_file_write_bool(_tf, false);
  text_file_end_record(_tf);
  text_file_delete(&_tf);
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ("true,false\n\nfalse\n", _s);
  string_delete(&_s);
  temp_filename_delete(&_tfn);
}

TEST(TextFile, WriteUlong) {
  char *_s = null;
  TempFilename *_tfn = null;
  TextFile *_tf = null;

  temp_filename_new(&_tfn, "csv");
  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  text_file_write_ulong(_tf, 0);
  text_file_write_ulong(_tf, 123456);
  text_file_end_record(_tf);
  text_file_end_record(_tf);
  text_file_write_ulong(_tf, ULONG_MAX);
  text_file_end_record(_tf);
  text_file_delete(&_tf);
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ("0,123456\n\n18446744073709551615\n", _s);
  string_delete(&_s);
  temp_filename_delete(&_tfn);
}

TEST(TextFile, WriteLong) {
  char *_s = null;
  TempFilename *_tfn = null;
  TextFile *_tf = null;

  temp_filename_new(&_tfn, "csv");
  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  text_file_write_long(_tf, 0);
  text_file_write_long(_tf, LONG_MIN);
  text_file_end_record(_tf);
  text_file_end_record(_tf);
  text_file_write_long(_tf, LONG_MAX);
  text_file_end_record(_tf);
  text_file_delete(&_tf);
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ("0,-9223372036854775808\n\n9223372036854775807\n", _s);
  string_delete(&_s);
  temp_filename_delete(&_tfn);
}

TEST(TextFile, WriteDouble) {
  char *_s = null;
  TempFilename *_tfn = null;
  TextFile *_tf = null;

  temp_filename_new(&_tfn, "csv");
  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  text_file_write_double(_tf, 0);
  text_file_write_double(_tf, 1.234);
  text_file_end_record(_tf);
  text_file_write_double(_tf, -0.01234567);
  text_file_end_record(_tf);
  text_file_write_double(_tf, DBL_MIN);
  text_file_write_double(_tf, -DBL_MAX);
  text_file_end_record(_tf);
  text_file_delete(&_tf);
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ(
      "0,1.234\n-0.01234567\n"
      "2.225073858507201e-308,-1.797693134862316e+308\n",
      _s);
  string_delete(&_s);
  temp_filename_delete(&_tfn);
}

TEST(TextFile, WriteEmptyField) {
  char *_s = null;
  TempFilename *_tfn = null;
  TextFile *_tf = null;

  temp_filename_new(&_tfn, "csv");
  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  text_file_write_empty_field(_tf);
  text_file_write(_tf, "dog");
  text_file_end_record(_tf);
  text_file_write_empty_field(_tf);
  text_file_end_record(_tf);
  text_file_write_empty_field(_tf);
  text_file_write_empty_field(_tf);
  text_file_write(_tf, "cat");
  text_file_end_record(_tf);
  text_file_delete(&_tf);
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ(",dog\n\n,,cat\n", _s);
  string_delete(&_s);
  temp_filename_delete(&_tfn);
}

TEST(TextFile, WriteEmptyRecord) {
  char *_s = null;
  TempFilename *_tfn = null;
  TextFile *_tf = null;

  temp_filename_new(&_tfn, "csv");
  text_file_new(&_tf, _tfn->_filename, "w");
  temp_filename_unlock(_tfn);
  text_file_write(_tf, "dog");
  text_file_end_record(_tf);
  text_file_write_empty_record(_tf);
  text_file_write(_tf, "cat");
  text_file_end_record(_tf);
  text_file_write_empty_record(_tf);
  text_file_write_empty_record(_tf);
  text_file_write(_tf, "woof");
  text_file_end_record(_tf);
  text_file_delete(&_tf);
  string_new_file_contents_from_filename(&_s, _tfn->_filename);
  EXPECT_STREQ("dog\n\ncat\n\n\nwoof\n", _s);
  string_delete(&_s);
  temp_filename_delete(&_tfn);
}

TEST(TextFile, WriteSingle) {
  char *_s = null;
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_single(_tf, "dog");
  text_file_write_single(_tf, "cat");
  text_file_write_empty_record(_tf);
  text_file_write_single(_tf, "woof");
  string_new_memory_file_contents(&_s, _tf->_file->_mf);
  text_file_delete(&_tf);
  EXPECT_STREQ("dog\ncat\n\nwoof\n", _s);
  string_delete(&_s);
}

TEST(TextFile, WriteSingleUlong) {
  char *_s = null;
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_single_ulong(_tf, 0);
  text_file_write_single_ulong(_tf, ULONG_MAX);
  text_file_write_empty_record(_tf);
  text_file_write_single_ulong(_tf, 24091966);
  string_new_memory_file_contents(&_s, _tf->_file->_mf);
  text_file_delete(&_tf);
  EXPECT_STREQ("0\n18446744073709551615\n\n24091966\n", _s);
  string_delete(&_s);
}

TEST(TextFile, WriteSingleLong) {
  char *_s = null;
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_single_long(_tf, 0);
  text_file_write_single_long(_tf, LONG_MAX);
  text_file_write_empty_record(_tf);
  text_file_write_single_long(_tf, LONG_MIN);
  string_new_memory_file_contents(&_s, _tf->_file->_mf);
  text_file_delete(&_tf);
  EXPECT_STREQ("0\n9223372036854775807\n\n-9223372036854775808\n", _s);
  string_delete(&_s);
}

TEST(TextFile, WriteSingleDouble) {
  char *_s = null;
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  EXPECT_ULEQ(',', _tf->separator);
  text_file_write_single_double(_tf, -0.012345);
  text_file_write_single_double(_tf, -DBL_MAX);
  text_file_write_empty_record(_tf);
  text_file_write_single_double(_tf, DBL_MIN);
  string_new_memory_file_contents(&_s, _tf->_file->_mf);
  text_file_delete(&_tf);
  EXPECT_STREQ(
      "-0.012345\n-1.797693134862316e+308\n\n"
      "2.225073858507201e-308\n",
      _s);
  string_delete(&_s);
}

TEST(TextFile, WriteLabel) {
  char *_s = null;
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_label(_tf, "dog");
  text_file_write_label(_tf, "cat");
  text_file_write_empty_record(_tf);
  text_file_write_label(_tf, "woof");
  string_new_memory_file_contents(&_s, _tf->_file->_mf);
  text_file_delete(&_tf);
  EXPECT_STREQ("dog\ncat\n\nwoof\n", _s);
  string_delete(&_s);
}

TEST(TextFile, WriteLabeled) {
  char *_s = null;
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_labeled(_tf, "dog", "cat");
  text_file_write_labeled(_tf, "cat", "miaow");
  text_file_write_empty_record(_tf);
  text_file_write_labeled(_tf, "woof", "snort");
  string_new_memory_file_contents(&_s, _tf->_file->_mf);
  text_file_delete(&_tf);
  EXPECT_STREQ("dog,cat\ncat,miaow\n\nwoof,snort\n", _s);
  string_delete(&_s);
}

TEST(TextFile, WriteLabeledBool) {
  char *_s = null;
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  EXPECT_ULEQ(',', _tf->separator);
  text_file_write_labeled_bool(_tf, "dog", true);
  text_file_write_labeled_bool(_tf, "cat", false);
  text_file_write_empty_record(_tf);
  text_file_write_labeled_bool(_tf, "woof", true);
  string_new_memory_file_contents(&_s, _tf->_file->_mf);
  text_file_delete(&_tf);
  EXPECT_STREQ("dog,true\ncat,false\n\nwoof,true\n", _s);
  string_delete(&_s);
}

TEST(TextFile, WriteLabeledByte) {
  char *_s = null;
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_labeled_byte(_tf, "dog", 255);
  text_file_write_labeled_byte(_tf, "cat", 0);
  text_file_write_empty_record(_tf);
  text_file_write_labeled_byte(_tf, "woof", 123);
  string_new_memory_file_contents(&_s, _tf->_file->_mf);
  text_file_delete(&_tf);
  EXPECT_STREQ("dog,255\ncat,0\n\nwoof,123\n", _s);
  string_delete(&_s);
}

TEST(TextFile, WriteLabeledUlong) {
  char *_s = null;
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_labeled_ulong(_tf, "dog", 123456);
  text_file_write_labeled_ulong(_tf, "cat", ULONG_MAX);
  text_file_write_empty_record(_tf);
  text_file_write_labeled_ulong(_tf, "woof", 0);
  string_new_memory_file_contents(&_s, _tf->_file->_mf);
  text_file_delete(&_tf);
  EXPECT_STREQ("dog,123456\ncat,18446744073709551615\n\nwoof,0\n", _s);
  string_delete(&_s);
}

TEST(TextFile, TryRead) {
  char **fields;
  ulong num_fields;
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_labeled_ulong(_tf, "dog", 123456);
  text_file_rewind(_tf);
  EXPECT_TRUE(text_file_try_read(_tf, &num_fields, &fields));
  EXPECT_ULEQ(2, num_fields);
  EXPECT_STREQ("dog", fields[0]);
  EXPECT_STREQ("123456", fields[1]);
  EXPECT_FALSE(text_file_try_read(_tf, &num_fields, &fields));
  text_file_delete(&_tf);
}

TEST(TextFile, ReadCopy) {
  char **_fields = null;
  ulong num_fields;
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_labeled_ulong(_tf, "dog", 123456);
  text_file_rewind(_tf);
  text_file_read_copy(_tf, &num_fields, &_fields);
  text_file_delete(&_tf);
  EXPECT_ULEQ(2, num_fields);
  EXPECT_STREQ("dog", _fields[0]);
  EXPECT_STREQ("123456", _fields[1]);
  delete(&_fields, 2, string_delete);
}

TEST(TextFile, TryReadCopy) {
  char **_fields = null;
  ulong num_fields;
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_labeled_ulong(_tf, "dog", 123456);
  text_file_rewind(_tf);
  EXPECT_TRUE(text_file_try_read_copy(_tf, &num_fields, &_fields));
  EXPECT_ULEQ(2, num_fields);
  EXPECT_STREQ("dog", _fields[0]);
  EXPECT_STREQ("123456", _fields[1]);
  delete(&_fields, 2, string_delete);
  EXPECT_FALSE(text_file_try_read_copy(_tf, &num_fields, &_fields));
  text_file_delete(&_tf);
}

TEST(TextFile, ReadDiscard) {
  char **fields;
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_labeled(_tf, "dog", "cat");
  text_file_write_labeled(_tf, "woof", "miaow");
  text_file_rewind(_tf);
  text_file_read_discard(_tf);
  EXPECT_ULEQ(2, text_file_read(_tf, &fields));
  EXPECT_STREQ("woof", fields[0]);
  EXPECT_STREQ("miaow", fields[1]);
  text_file_delete(&_tf);
}

TEST(TextFile, ReadEmptyRecord) {
  char **fields;
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_labeled(_tf, "dog", "cat");
  text_file_write_empty_record(_tf);
  text_file_write_labeled(_tf, "woof", "miaow");
  text_file_rewind(_tf);
  text_file_read_discard(_tf);
  text_file_read_empty_record(_tf);
  EXPECT_ULEQ(2, text_file_read(_tf, &fields));
  EXPECT_STREQ("woof", fields[0]);
  EXPECT_STREQ("miaow", fields[1]);
  text_file_delete(&_tf);
}

TEST(TextFile, ReadSingle) {
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_single(_tf, "dog");
  text_file_write_empty_record(_tf);
  text_file_write_single(_tf, "cat");
  text_file_rewind(_tf);
  EXPECT_STREQ("dog", text_file_read_single(_tf));
  text_file_read_empty_record(_tf);
  EXPECT_STREQ("cat", text_file_read_single(_tf));
  text_file_delete(&_tf);
}

TEST(TextFile, ReadSingleUlong) {
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_single_ulong(_tf, 123);
  text_file_write_empty_record(_tf);
  text_file_write_single_ulong(_tf, ULONG_MAX);
  text_file_rewind(_tf);
  EXPECT_ULEQ(123, text_file_read_single_ulong(_tf));
  text_file_read_empty_record(_tf);
  EXPECT_ULEQ(ULONG_MAX, text_file_read_single_ulong(_tf));
  text_file_delete(&_tf);
}

TEST(TextFile, ReadLabel) {
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_label(_tf, "dog");
  text_file_write_empty_record(_tf);
  text_file_write_label(_tf, "cat");
  text_file_rewind(_tf);
  text_file_read_label(_tf, "dog");
  text_file_read_empty_record(_tf);
  text_file_read_label(_tf, "cat");
  text_file_delete(&_tf);
}

TEST(TextFile, ReadLabeled) {
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_labeled(_tf, "dog", "woof");
  text_file_write_empty_record(_tf);
  text_file_write_labeled(_tf, "cat", "miaow");
  text_file_rewind(_tf);
  EXPECT_STREQ("woof", text_file_read_labeled(_tf, "dog"));
  text_file_read_empty_record(_tf);
  EXPECT_STREQ("miaow", text_file_read_labeled(_tf, "cat"));
  text_file_delete(&_tf);
}

TEST(TextFile, ReadLabeledBool) {
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_labeled_bool(_tf, "dog", true);
  text_file_write_empty_record(_tf);
  text_file_write_labeled_bool(_tf, "cat", false);
  text_file_rewind(_tf);
  EXPECT_TRUE(text_file_read_labeled_bool(_tf, "dog"));
  text_file_read_empty_record(_tf);
  EXPECT_FALSE(text_file_read_labeled_bool(_tf, "cat"));
  text_file_delete(&_tf);
}

TEST(TextFile, ReadLabeledByte) {
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_labeled_byte(_tf, "dog", 255);
  text_file_write_empty_record(_tf);
  text_file_write_labeled_byte(_tf, "cat", 0);
  text_file_rewind(_tf);
  EXPECT_ULEQ(255, text_file_read_labeled_byte(_tf, "dog"));
  text_file_read_empty_record(_tf);
  EXPECT_ULEQ(0, text_file_read_labeled_byte(_tf, "cat"));
  text_file_delete(&_tf);
}

TEST(TextFile, ReadLabeledUlong) {
  TextFile *_tf = null;

  text_file_new_memory(&_tf, "csv");
  text_file_write_labeled_ulong(_tf, "dog", 123456);
  text_file_write_empty_record(_tf);
  text_file_write_labeled_ulong(_tf, "cat", ULONG_MAX);
  text_file_rewind(_tf);
  EXPECT_ULEQ(123456, text_file_read_labeled_ulong(_tf, "dog"));
  text_file_read_empty_record(_tf);
  EXPECT_ULEQ(ULONG_MAX, text_file_read_labeled_ulong(_tf, "cat"));
  text_file_delete(&_tf);
}

FINISH


static void record_delete_(Record_ **p_this) {
  ulong f;
  Record_ *this;

  this = *p_this;

  for (f = 0; f < this->numFields; f++) {
    deletev(&this->_fields[f]);
  }

  deletev(&this->_fieldLengths);
  deletev(&this->_fields);

  deleteo(p_this);
}
