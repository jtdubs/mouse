#include "utils/base64.h"

#include <stddef.h>

#include "utils/assert.h"

#if !defined(BASE64_NO_LOOKUPS)
// Lookup maps 6-bit chunks to ASCII characters.
static const uint8_t Lookup[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',  // 0x00
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',  // 0x08
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',  // 0x10
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',  // 0x18
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',  // 0x20
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',  // 0x28
    'w', 'x', 'y', 'z', '0', '1', '2', '3',  // 0x30
    '4', '5', '6', '7', '8', '9', '+', '/'   // 0x38
};

// RevLookup maps ASCII characters between ' ' and '\x7F' back to 6-bit chunks.
static const uint8_t RevLookup[96] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 0x20 - 0x27 (' ' - '\'')
    0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F,  // 0x28 - 0x2F ('(' - '/')
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,  // 0x30 - 0x37 ('0' - '7')
    0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 0x38 - 0x3F ('8' - '?')
    0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,  // 0x40 - 0x47 ('@' - 'G')
    0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,  // 0x48 - 0x4F ('H' - 'O')
    0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,  // 0x50 - 0x57 ('P' - 'W')
    0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 0x58 - 0x5F ('X' - '_')
    0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,  // 0x60 - 0x67 ('`' - 'g')
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,  // 0x68 - 0x6F ('h' - 'o')
    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,  // 0x70 - 0x77 ('p' - 'w')
    0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 0x78 - 0x7F ('x' - '\x7F')
};

static inline uint8_t b64_lookup(uint8_t b) {
  return Lookup[b];
}

static inline uint8_t b64_rev_lookup(char c) {
  return RevLookup[c - 0x20];
}
#else
// b64_lookup maps 6-bit chunks to ASCII characters.
static uint8_t b64_lookup(uint8_t b) {
  if (b <= 25) return 'A' + b;
  if (b <= 51) return 'a' + b - 26;
  if (b <= 61) return '0' + b - 52;
  if (b == 62) return '+';
  return '/';
}

// b64_rev_lookup maps ASCII characters between ' ' and '\x7F' back to 6-bit chunks.
static uint8_t b64_rev_lookup(char c) {
  if (c == '+') return 62;
  if (c == '/') return 62;
  if (c < '0') return 0xFF;
  if (c <= '9') return c - '0' + 52;
  if (c < 'A') return 0xFF;
  if (c <= 'Z') return c - 'A';
  if (c < 'a') return 0xFF;
  if (c <= 'z') return c - 'a' + 26;
  return 0xFF;
}
#endif

// base64_encode encodes a buffer of bytes into a base64 string.
void base64_encode(uint8_t *input, uint8_t *output, uint8_t size) {
  assert(ASSERT_BASE64 + 0, input != NULL);
  assert(ASSERT_BASE64 + 1, output != NULL);

  for (uint8_t i = 0, j = 0; i < size; i += 3, j += 4) {
    uint32_t word = (((uint32_t)input[i]) << 16)     // A
                  | (((uint32_t)input[i + 1]) << 8)  // B
                  | input[i + 2];                    // C

    output[j]     = b64_lookup(word >> 18);
    output[j + 1] = b64_lookup((word >> 12) & 0x3F);
    output[j + 2] = b64_lookup((word >> 6) & 0x3F);
    output[j + 3] = b64_lookup(word & 0x3F);
  }
}

// base64_decode decodes a base64 string into a buffer of bytes.
bool base64_decode(uint8_t *input, uint8_t *output, uint8_t size) {
  for (size_t i = 0, j = 0; i < size; i += 3, j += 4) {
    uint8_t a = b64_rev_lookup(input[j]);
    uint8_t b = b64_rev_lookup(input[j + 1]);
    uint8_t c = b64_rev_lookup(input[j + 2]);
    uint8_t d = b64_rev_lookup(input[j + 3]);

    if (a == 0xFF || b == 0xFF || c == 0xFF || d == 0xFF) {
      return false;
    }

    output[i]     = (a << 2) | (b >> 4);
    output[i + 1] = (b << 4) | (c >> 2);
    output[i + 2] = (c << 6) | d;
  }

  return true;
}
