#pragma once

#include <stdint.h>

// base64_encode encodes a buffer of bytes into a base64 string.
void base64_encode(uint8_t *input, uint8_t *output, uint8_t size);
