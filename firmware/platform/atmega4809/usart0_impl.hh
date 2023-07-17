#pragma once

#include <stdint.h>

#include "firmware/platform/usart0.hh"

constexpr uint8_t START_BYTE    = 0x02;
constexpr uint8_t MAX_READ_SIZE = 64;

// Write states
typedef enum : uint8_t {
  WRITE_IDLE,
  WRITE_START,
  WRITE_LENGTH,
  WRITE_DATA,
  WRITE_CHECKSUM,
} write_state_t;

// Read states
typedef enum : uint8_t {
  READ_IDLE,
  READ_LENGTH,
  READ_DATA,
  READ_CHECKSUM,
} read_state_t;
