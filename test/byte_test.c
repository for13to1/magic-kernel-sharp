#include "byte_test.h"

START

TEST(Byte, Bound) {
  EXPECT_ULEQ(0, byte_bound(-1e30));
  EXPECT_ULEQ(0, byte_bound(-123));
  EXPECT_ULEQ(0, byte_bound(-1));
  EXPECT_ULEQ(0, byte_bound(-1e-6));
  EXPECT_ULEQ(0, byte_bound(0));
  EXPECT_ULEQ(0, byte_bound(0.5 - 1e-6));
  EXPECT_ULEQ(1, byte_bound(0.5 + 1e-6));
  EXPECT_ULEQ(254, byte_bound(254.5 - 1e-6));
  EXPECT_ULEQ(255, byte_bound(254.5 + 1e-6));
  EXPECT_ULEQ(255, byte_bound(255));
  EXPECT_ULEQ(255, byte_bound(256));
  EXPECT_ULEQ(255, byte_bound(260));
  EXPECT_ULEQ(255, byte_bound(1000));
  EXPECT_ULEQ(255, byte_bound(1e30));
}

TEST(Byte, Diff) {
  byte by;
  short a;
  short d;
  short d_q;
  short e;
  double diff;
  double diff_q;

  for (d = -255; d <= +255; d++) {
    diff = d / 255.0;
    by = byte_encode_diff(diff);
    diff_q = byte_decode_diff(by);
    d_q = (short)round(diff_q * 255);
    a = abs(d);

    switch (a) {
      case 26: case 33: case 39: case 44: case 48: case 51: case 55: case 58:
      case 61: case 64: case 67: case 69: case 72: case 74: case 77: case 79:
      case 81: case 84: case 86: case 88: case 90: case 92: case 94: case 96:
      case 98: case 100: case 102: case 104: case 106: case 108: case 110:
      case 113: case 115: case 117: case 120: case 122: case 124: case 127:
      case 129: case 132: case 134: case 137: case 139: case 142: case 144:
      case 147: case 150: case 153: case 155: case 158: case 161: case 164:
      case 167: case 170: case 173: case 176: case 179: case 182: case 185:
      case 188: case 192: case 195: case 198: case 202: case 205: case 209:
      case 212: case 216: case 219: case 223: case 226: case 230: case 234:
      case 238: case 242: case 246: case 250: case 253:
        e = a + 1;
        break;

      case 112: case 119: case 126: case 131: case 136: case 141: case 146:
      case 149: case 152: case 157: case 160: case 163: case 166: case 169:
      case 172: case 175: case 178: case 181: case 184: case 187: case 190:
      case 194: case 197: case 200: case 204: case 207: case 211: case 214:
      case 218: case 221: case 225: case 228: case 232: case 236: case 240:
      case 244: case 248: case 252: case 255:
        e = a - 1;
        break;

      case 191: case 201: case 208: case 215: case 222: case 229: case 233:
      case 237: case 241: case 245: case 249:
        e = a + 2;
        break;

      default:
        e = a;
        break;
    }

    if (d < 0) {
      e = -e;
    }

    EXPECT_LEQ(e, d_q);
  }
}

FINISH
