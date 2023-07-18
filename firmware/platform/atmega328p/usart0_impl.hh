#pragma once

#include <stdint.h>

#include "firmware/platform/usart0.hh"

namespace usart0 {

constexpr uint8_t kStartByte   = 0x02;
constexpr uint8_t kMaxReadSize = 64;

// Write states
enum class WriteState : uint8_t {
  Idle,
  Start,
  Length,
  Data,
  Checksum,
};

// Read states
enum class ReadState : uint8_t {
  Idle,
  Length,
  Data,
  Checksum,
};

}
