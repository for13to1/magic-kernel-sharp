#include "md5_test.h"

START

TEST(MD5, String) {
  char *_s = null;
  char *e = "d41d8cd98f00b204e9800998ecf8427e";

  string_new_md5(&_s, 0, (byte *)"");
  EXPECT_STREQ(e, _s);
  string_delete(&_s);

  e = "363b122c528f54df4a0446b6bab05515";
  string_new_md5(&_s, 1, (byte *)"j");
  EXPECT_STREQ(e, _s);
  string_delete(&_s);

  e = "363b122c528f54df4a0446b6bab05515";
  string_new_md5(&_s, 1, (byte *)"j");
  EXPECT_STREQ(e, _s);
  string_delete(&_s);

  e = "55add3d845bfcd87a9b0949b0da49c0a";
  string_new_md5(&_s, 2, (byte *)"jp");
  EXPECT_STREQ(e, _s);
  string_delete(&_s);
}

TEST(MD5, Ulongs) {
  ulong hi;
  ulong lo;

  md5_ulongs(0, (byte *)"", &hi, &lo);
  EXPECT_ULEQ(0xd41d8cd98f00b204, hi);
  EXPECT_ULEQ(0xe9800998ecf8427e, lo);

  md5_ulongs(1, (byte *)"j", &hi, &lo);
  EXPECT_ULEQ(0x363b122c528f54df, hi);
  EXPECT_ULEQ(0x4a0446b6bab05515, lo);

  md5_ulongs(2, (byte *)"jp", &hi, &lo);
  EXPECT_ULEQ(0x55add3d845bfcd87, hi);
  EXPECT_ULEQ(0xa9b0949b0da49c0a, lo);
}

TEST(MD5, Bytes) {
  byte b[16];

  md5_bytes(0, (byte *)"", b);
  EXPECT_ULEQ(0xd4, b[0]);
  EXPECT_ULEQ(0xd9, b[3]);
  EXPECT_ULEQ(0xb2, b[6]);
  EXPECT_ULEQ(0x80, b[9]);
  EXPECT_ULEQ(0xec, b[12]);
  EXPECT_ULEQ(0x7e, b[15]);

  md5_bytes(1, (byte *)"j", b);
  EXPECT_ULEQ(0x3b, b[1]);
  EXPECT_ULEQ(0x52, b[4]);
  EXPECT_ULEQ(0xdf, b[7]);
  EXPECT_ULEQ(0x46, b[10]);
  EXPECT_ULEQ(0xb0, b[13]);

  md5_bytes(2, (byte *)"jp", b);
  EXPECT_ULEQ(0xd3, b[2]);
  EXPECT_ULEQ(0xbf, b[5]);
  EXPECT_ULEQ(0xa9, b[8]);
  EXPECT_ULEQ(0x9b, b[11]);
  EXPECT_ULEQ(0x9c, b[14]);
}

TEST(MD5, BytesToUlongs) {
  byte b[16] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
  };
  ulong hi;
  ulong lo;

  md5_bytes_to_ulongs(b, &hi, &lo);
  EXPECT_ULEQ(0x0123456789abcdef, hi);
  EXPECT_ULEQ(0xfedcba9876543210, lo);
}

TEST(MD5, StringFromUlongs) {
  char *_s = null;
  char *e = "0123456789abcdeffedcba9876543210";

  string_new_md5_from_ulongs(&_s, 0x0123456789abcdef, 0xfedcba9876543210);
  EXPECT_STREQ(e, _s);
  string_delete(&_s);
}

FINISH
