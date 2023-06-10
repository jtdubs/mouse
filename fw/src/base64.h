#pragma once

#include <stdbool.h>
#include <stdint.h>

// base64_encode encodes a buffer of bytes into a base64 string.
void base64_encode(uint8_t *input, uint8_t *output, uint8_t size);

// base64_decode decodes a base64 string into a buffer of bytes.
bool base64_decode(uint8_t *input, uint8_t *output, uint8_t size);
