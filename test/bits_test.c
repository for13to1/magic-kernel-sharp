#include "bits_test.h"

START

TEST(Byte, BitWidth) {
  EXPECT_ULEQ(0, byte_bit_width(0));
  EXPECT_ULEQ(1, byte_bit_width(1));
  EXPECT_ULEQ(2, byte_bit_width(2));
  EXPECT_ULEQ(2, byte_bit_width(3));
  EXPECT_ULEQ(3, byte_bit_width(4));
  EXPECT_ULEQ(3, byte_bit_width(7));
  EXPECT_ULEQ(4, byte_bit_width(8));
  EXPECT_ULEQ(4, byte_bit_width(0xf));
  EXPECT_ULEQ(5, byte_bit_width(0x10));
  EXPECT_ULEQ(5, byte_bit_width(0x1f));
  EXPECT_ULEQ(6, byte_bit_width(0x20));
  EXPECT_ULEQ(6, byte_bit_width(0x3f));
  EXPECT_ULEQ(7, byte_bit_width(0x40));
  EXPECT_ULEQ(7, byte_bit_width(0x7f));
  EXPECT_ULEQ(8, byte_bit_width(0x80));
  EXPECT_ULEQ(8, byte_bit_width(0xff));
}

TEST(Ushort, BitWidth) {
  EXPECT_ULEQ(0, ushort_bit_width(0));
  EXPECT_ULEQ(1, ushort_bit_width(1));
  EXPECT_ULEQ(2, ushort_bit_width(2));
  EXPECT_ULEQ(2, ushort_bit_width(3));
  EXPECT_ULEQ(3, ushort_bit_width(4));
  EXPECT_ULEQ(3, ushort_bit_width(7));
  EXPECT_ULEQ(4, ushort_bit_width(8));
  EXPECT_ULEQ(4, ushort_bit_width(0xf));
  EXPECT_ULEQ(5, ushort_bit_width(0x10));
  EXPECT_ULEQ(5, ushort_bit_width(0x1f));
  EXPECT_ULEQ(8, ushort_bit_width(0xff));
  EXPECT_ULEQ(9, ushort_bit_width(0x100));
  EXPECT_ULEQ(16, ushort_bit_width(0x8000));
  EXPECT_ULEQ(16, ushort_bit_width(0xffff));
}

TEST(Uint, BitWidth) {
  EXPECT_ULEQ(0, uint_bit_width(0));
  EXPECT_ULEQ(1, uint_bit_width(1));
  EXPECT_ULEQ(2, uint_bit_width(2));
  EXPECT_ULEQ(2, uint_bit_width(3));
  EXPECT_ULEQ(3, uint_bit_width(4));
  EXPECT_ULEQ(3, uint_bit_width(7));
  EXPECT_ULEQ(4, uint_bit_width(8));
  EXPECT_ULEQ(4, uint_bit_width(0xf));
  EXPECT_ULEQ(5, uint_bit_width(0x10));
  EXPECT_ULEQ(5, uint_bit_width(0x1f));
  EXPECT_ULEQ(8, uint_bit_width(0xff));
  EXPECT_ULEQ(9, uint_bit_width(0x100));
  EXPECT_ULEQ(16, uint_bit_width(0xffff));
  EXPECT_ULEQ(17, uint_bit_width(0x10000));
  EXPECT_ULEQ(32, uint_bit_width(0x80000000));
  EXPECT_ULEQ(32, uint_bit_width(0xffffffff));
}

TEST(Ulong, BitWidth) {
  EXPECT_ULEQ(0, ulong_bit_width(0));
  EXPECT_ULEQ(1, ulong_bit_width(1));
  EXPECT_ULEQ(2, ulong_bit_width(2));
  EXPECT_ULEQ(2, ulong_bit_width(3));
  EXPECT_ULEQ(3, ulong_bit_width(4));
  EXPECT_ULEQ(3, ulong_bit_width(7));
  EXPECT_ULEQ(4, ulong_bit_width(8));
  EXPECT_ULEQ(4, ulong_bit_width(0xf));
  EXPECT_ULEQ(5, ulong_bit_width(0x10));
  EXPECT_ULEQ(5, ulong_bit_width(0x1f));
  EXPECT_ULEQ(8, ulong_bit_width(0xff));
  EXPECT_ULEQ(9, ulong_bit_width(0x100));
  EXPECT_ULEQ(16, ulong_bit_width(0xffff));
  EXPECT_ULEQ(17, ulong_bit_width(0x10000));
  EXPECT_ULEQ(32, ulong_bit_width(0xffffffff));
  EXPECT_ULEQ(33, ulong_bit_width(0x100000000));
  EXPECT_ULEQ(64, ulong_bit_width(0x8000000000000000));
  EXPECT_ULEQ(64, ulong_bit_width(0xffffffffffffffff));
}

TEST(Ulong, HexitWidth) {
  EXPECT_ULEQ(0, ulong_hexit_width(0));
  EXPECT_ULEQ(1, ulong_hexit_width(1));
  EXPECT_ULEQ(1, ulong_hexit_width(0xf));
  EXPECT_ULEQ(2, ulong_hexit_width(0x10));
  EXPECT_ULEQ(2, ulong_hexit_width(0xff));
  EXPECT_ULEQ(3, ulong_hexit_width(0x100));
  EXPECT_ULEQ(3, ulong_hexit_width(0xfff));
  EXPECT_ULEQ(4, ulong_hexit_width(0x1000));
  EXPECT_ULEQ(4, ulong_hexit_width(0xffff));
  EXPECT_ULEQ(5, ulong_hexit_width(0x10000));
  EXPECT_ULEQ(8, ulong_hexit_width(0xffffffff));
  EXPECT_ULEQ(9, ulong_hexit_width(0x100000000));
  EXPECT_ULEQ(16, ulong_hexit_width(0xffffffffffffffff));
}

TEST(Byte, ReverseBits) {
  ushort u;

  EXPECT_ULEQ(0x00, byte_reverse_bits(0x00));

  EXPECT_ULEQ(0x80, byte_reverse_bits(0x01));
  EXPECT_ULEQ(0x40, byte_reverse_bits(0x02));
  EXPECT_ULEQ(0x20, byte_reverse_bits(0x04));
  EXPECT_ULEQ(0x10, byte_reverse_bits(0x08));

  EXPECT_ULEQ(0xc0, byte_reverse_bits(0x03));
  EXPECT_ULEQ(0xe0, byte_reverse_bits(0x07));
  EXPECT_ULEQ(0xf0, byte_reverse_bits(0x0f));
  EXPECT_ULEQ(0xf8, byte_reverse_bits(0x1f));
  EXPECT_ULEQ(0xfc, byte_reverse_bits(0x3f));
  EXPECT_ULEQ(0xfe, byte_reverse_bits(0x7f));
  EXPECT_ULEQ(0xff, byte_reverse_bits(0xff));

  EXPECT_ULEQ(0xa0, byte_reverse_bits(0x05));
  EXPECT_ULEQ(0x50, byte_reverse_bits(0x0a));

  for (u = 0; u < 0x100; u++) {
    EXPECT_ULEQ(u, byte_reverse_bits(byte_reverse_bits(u)));
  }
}

TEST(Ushort, ReverseBits) {
  uint u;

  EXPECT_ULEQ(0x0000, ushort_reverse_bits(0x0000));

  EXPECT_ULEQ(0x8000, ushort_reverse_bits(0x0001));
  EXPECT_ULEQ(0x4000, ushort_reverse_bits(0x0002));
  EXPECT_ULEQ(0x2000, ushort_reverse_bits(0x0004));
  EXPECT_ULEQ(0x1000, ushort_reverse_bits(0x0008));
  EXPECT_ULEQ(0x0800, ushort_reverse_bits(0x0010));
  EXPECT_ULEQ(0x0400, ushort_reverse_bits(0x0020));
  EXPECT_ULEQ(0x0200, ushort_reverse_bits(0x0040));
  EXPECT_ULEQ(0x0100, ushort_reverse_bits(0x0080));

  EXPECT_ULEQ(0xc000, ushort_reverse_bits(0x0003));
  EXPECT_ULEQ(0xe000, ushort_reverse_bits(0x0007));
  EXPECT_ULEQ(0xf000, ushort_reverse_bits(0x000f));
  EXPECT_ULEQ(0xf800, ushort_reverse_bits(0x001f));
  EXPECT_ULEQ(0xfc00, ushort_reverse_bits(0x003f));
  EXPECT_ULEQ(0xfe00, ushort_reverse_bits(0x007f));
  EXPECT_ULEQ(0xff00, ushort_reverse_bits(0x00ff));
  EXPECT_ULEQ(0xff80, ushort_reverse_bits(0x01ff));
  EXPECT_ULEQ(0xffc0, ushort_reverse_bits(0x03ff));
  EXPECT_ULEQ(0xffe0, ushort_reverse_bits(0x07ff));
  EXPECT_ULEQ(0xfff0, ushort_reverse_bits(0x0fff));
  EXPECT_ULEQ(0xfff8, ushort_reverse_bits(0x1fff));
  EXPECT_ULEQ(0xfffc, ushort_reverse_bits(0x3fff));
  EXPECT_ULEQ(0xfffe, ushort_reverse_bits(0x7fff));
  EXPECT_ULEQ(0xffff, ushort_reverse_bits(0xffff));

  EXPECT_ULEQ(0x6a59, ushort_reverse_bits(0x9a56));

  for (u = 0; u < 0x10000; u++) {
    EXPECT_ULEQ(u, ushort_reverse_bits(ushort_reverse_bits(u)));
  }
}

TEST(Uint, ReverseBits) {
  ulong u;

  EXPECT_ULEQ(0x00000000, uint_reverse_bits(0x00000000));

  EXPECT_ULEQ(0x80000000, uint_reverse_bits(0x00000001));
  EXPECT_ULEQ(0x40000000, uint_reverse_bits(0x00000002));
  EXPECT_ULEQ(0x20000000, uint_reverse_bits(0x00000004));
  EXPECT_ULEQ(0x10000000, uint_reverse_bits(0x00000008));
  EXPECT_ULEQ(0x00800000, uint_reverse_bits(0x00000100));
  EXPECT_ULEQ(0x00400000, uint_reverse_bits(0x00000200));
  EXPECT_ULEQ(0x00200000, uint_reverse_bits(0x00000400));
  EXPECT_ULEQ(0x00100000, uint_reverse_bits(0x00000800));

  EXPECT_ULEQ(0xc0000000, uint_reverse_bits(0x00000003));
  EXPECT_ULEQ(0xe0000000, uint_reverse_bits(0x00000007));
  EXPECT_ULEQ(0xf0000000, uint_reverse_bits(0x0000000f));
  EXPECT_ULEQ(0xff800000, uint_reverse_bits(0x000001ff));
  EXPECT_ULEQ(0xffc00000, uint_reverse_bits(0x000003ff));
  EXPECT_ULEQ(0xffe00000, uint_reverse_bits(0x000007ff));
  EXPECT_ULEQ(0xfff00000, uint_reverse_bits(0x00000fff));
  EXPECT_ULEQ(0xffff8000, uint_reverse_bits(0x0001ffff));
  EXPECT_ULEQ(0xffffc000, uint_reverse_bits(0x0003ffff));
  EXPECT_ULEQ(0xffffe000, uint_reverse_bits(0x0007ffff));
  EXPECT_ULEQ(0xfffff000, uint_reverse_bits(0x000fffff));
  EXPECT_ULEQ(0xfffffff8, uint_reverse_bits(0x1fffffff));
  EXPECT_ULEQ(0xfffffffc, uint_reverse_bits(0x3fffffff));
  EXPECT_ULEQ(0xfffffffe, uint_reverse_bits(0x7fffffff));
  EXPECT_ULEQ(0xffffffff, uint_reverse_bits(0xffffffff));

  EXPECT_ULEQ(0x6a59817c, uint_reverse_bits(0x3e819a56));

  for (u = 0; u < 0x100000000; u += 0x12345) {
    EXPECT_ULEQ(u, uint_reverse_bits(uint_reverse_bits(u)));
  }
}

TEST(Ulong, ReverseBits) {
  ulong u;
  uint v;

  EXPECT_ULEQ(0x0000000000000000, ulong_reverse_bits(0x0000000000000000));

  EXPECT_ULEQ(0x0008000000000000, ulong_reverse_bits(0x0000000000001000));
  EXPECT_ULEQ(0x0004000000000000, ulong_reverse_bits(0x0000000000002000));
  EXPECT_ULEQ(0x0002000000000000, ulong_reverse_bits(0x0000000000004000));
  EXPECT_ULEQ(0x0001000000000000, ulong_reverse_bits(0x0000000000008000));
  EXPECT_ULEQ(0x0000000800000000, ulong_reverse_bits(0x0000000010000000));
  EXPECT_ULEQ(0x0000000400000000, ulong_reverse_bits(0x0000000020000000));
  EXPECT_ULEQ(0x0000000200000000, ulong_reverse_bits(0x0000000040000000));
  EXPECT_ULEQ(0x0000000100000000, ulong_reverse_bits(0x0000000080000000));

  EXPECT_ULEQ(0x0c00000000000000, ulong_reverse_bits(0x0000000000000030));
  EXPECT_ULEQ(0x0e00000000000000, ulong_reverse_bits(0x0000000000000070));
  EXPECT_ULEQ(0x0f00000000000000, ulong_reverse_bits(0x00000000000000f0));
  EXPECT_ULEQ(0x0f0f080000000000, ulong_reverse_bits(0x000000000010f0f0));
  EXPECT_ULEQ(0x0f0f0c0000000000, ulong_reverse_bits(0x000000000030f0f0));
  EXPECT_ULEQ(0x0f0f0e0000000000, ulong_reverse_bits(0x000000000070f0f0));
  EXPECT_ULEQ(0x0f0f0f0000000000, ulong_reverse_bits(0x0000000000f0f0f0));
  EXPECT_ULEQ(0x0f0f0f0f08000000, ulong_reverse_bits(0x00000010f0f0f0f0));
  EXPECT_ULEQ(0x0f0f0f0f0c000000, ulong_reverse_bits(0x00000030f0f0f0f0));
  EXPECT_ULEQ(0x0f0f0f0f0e000000, ulong_reverse_bits(0x00000070f0f0f0f0));
  EXPECT_ULEQ(0x0f0f0f0f0f000000, ulong_reverse_bits(0x000000f0f0f0f0f0));
  EXPECT_ULEQ(0x0f0f0f0f0f0f0f08, ulong_reverse_bits(0x10f0f0f0f0f0f0f0));
  EXPECT_ULEQ(0x0f0f0f0f0f0f0f0c, ulong_reverse_bits(0x30f0f0f0f0f0f0f0));
  EXPECT_ULEQ(0x0f0f0f0f0f0f0f0e, ulong_reverse_bits(0x70f0f0f0f0f0f0f0));
  EXPECT_ULEQ(0x0f0f0f0f0f0f0f0f, ulong_reverse_bits(0xf0f0f0f0f0f0f0f0));

  EXPECT_ULEQ(0x6a59817c3e819a56, ulong_reverse_bits(0x6a59817c3e819a56));

  for (u = 0, v = 0; v < 0x10000; u += 0x123456789, v++) {
    EXPECT_ULEQ(u, ulong_reverse_bits(ulong_reverse_bits(u)));
  }
}

FINISH
