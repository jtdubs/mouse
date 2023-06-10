#include "base64.h"

#include <assert.h>
#include <stddef.h>

static const uint8_t b64_lookup[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',  // 0x00
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',  // 0x08
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',  // 0x10
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',  // 0x18
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',  // 0x20
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',  // 0x28
    'w', 'x', 'y', 'z', '0', '1', '2', '3',  // 0x30
    '4', '5', '6', '7', '8', '9', '+', '/'   // 0x38
};

// base64_encode encodes a buffer of bytes into a base64 string.
void base64_encode(uint8_t *input, uint8_t *output, uint8_t size) {
  assert(input != NULL);
  assert(output != NULL);
  assert(size > 0 && size % 3 == 0);

  for (uint8_t i = 0, j = 0; i < size; i += 3, j += 4) {
    // Pack next three input bytes {A,B,C} into a 24-bit word 0xABC.
    uint32_t word = (((uint32_t)input[i]) << 16)     // A
                  | (((uint32_t)input[i + 1]) << 8)  // B
                  | input[i + 2];                    // C

    // Unpack each 6-bits of the word, and encode.
    output[j]     = b64_lookup[word >> 18];
    output[j + 1] = b64_lookup[(word >> 12) & 0x3F];
    output[j + 2] = b64_lookup[(word >> 6) & 0x3F];
    output[j + 3] = b64_lookup[word & 0x3F];
  }
}
