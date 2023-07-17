//
// System: utils
// Module: assert
//
// Purpose:
// - Defines the system assert macro
// - Provides an error state which visually blinks the error code to the user.
//
#pragma once

#include <stdint.h>

#if !defined(NOASSERT)
// assert asserts that the expression is true, and does not return if it is false.
#define assert(n, e)     \
  {                      \
    if (!(e)) {          \
      assert::failed(n); \
    }                    \
  }
#else
#define assert(n, e)
#endif

namespace assert {

// range_t defines the ranges of assert codes available to each module.
enum range_t : uint32_t {
  MAIN         = 0x01000000,
  USART0_WRITE = 0x02000000,
  USART0_READ  = 0x03000000,
  COMMAND      = 0x04000000,
  PLAN         = 0x05000000,
  MAZE         = 0x06000000,
  MOTOR        = 0x07000000,
  PLATFORM     = 0x08000000,
  CONTROL      = 0x09000000,
  EXPLORE      = 0x0A000000,
  _ADC         = 0x0B000000,
  TIMER        = 0x0C000000,
  ENCODERS     = 0x0D000000,
  SPEED        = 0x0E000000,
  WALLS        = 0x0F000000,
  SENSOR       = 0x10000000,
  POSITION     = 0x20000000,
  LINEAR       = 0x30000000,
  DEQUEUE      = 0x40000000,
};

// failed is a permanent error state.
void failed(uint32_t error);

}  // namespace assert
