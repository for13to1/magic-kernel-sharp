#include "bits.h"

byte byte_bit_width(byte b) {
  static bool initialized__ = false;
  static Lock lock__ = LOCK_INITIALIZER;
  static byte widths__[0x100];

  byte width;
  ushort i;
  ushort j;

  if (!initialized__) {
    lock(&lock__);

    if (!initialized__) {
      widths__[0] = width = 0;

      for (j = 0, i = 1; i < 0x100; j = i, i++) {
        if ((i & j) == 0) {
          width++;
        }

        widths__[i] = width;
      }

      initialized__ = true;
    }

    unlock(&lock__);
  }

  return widths__[b];
}

byte ushort_bit_width(ushort u) {
  if ((u & 0xff00) != 0) {
    return byte_bit_width(u >> 8) + 8;
  }

  return byte_bit_width((byte)u);
}

byte uint_bit_width(uint u) {
  if ((u & 0xffff0000) != 0) {
    return ushort_bit_width(u >> 16) + 16;
  }

  return ushort_bit_width((ushort)u);
}

byte ulong_bit_width(ulong u) {
  if ((u & 0xffffffff00000000) != 0) {
    return ulong_bit_width(u >> 32) + 32;
  }

  return uint_bit_width((uint)u);
}

byte ulong_hexit_width(ulong u) {
  return (ulong_bit_width(u) + 3) >> 2;
}

byte byte_reverse_bits(byte b) {
  static bool initialized__ = false;
  static Lock lock__ = LOCK_INITIALIZER;
  static byte reverses__[0x100];

  byte i;
  byte j;
  byte k;
  ushort u;

  if (!initialized__) {
    lock(&lock__);

    if (!initialized__) {
      for (u = 0; u < 0x100; u++) {
        for (i = u >> 1, j = 7, k = u; i; j--, k <<= 1, k |= i & 1, i >>= 1) {
        }

        reverses__[u] = k << j;
      }

      initialized__ = true;
    }

    unlock(&lock__);
  }

  return reverses__[b];
}

ushort ushort_reverse_bits(ushort u) {
  return (ushort)byte_reverse_bits(u) << 8 | byte_reverse_bits(u >> 8);
}

uint uint_reverse_bits(uint u) {
  return (uint)ushort_reverse_bits(u) << 16 | ushort_reverse_bits(u >> 16);
}

ulong ulong_reverse_bits(ulong u) {
  return (ulong)uint_reverse_bits(u) << 32 | uint_reverse_bits(u >> 32);
}
