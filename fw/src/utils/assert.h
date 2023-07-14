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

// assert_range_t defines the ranges of assert codes available to each module.
enum assert_range_t : uint32_t {
  ASSERT_MAIN         = 0x01000000,
  ASSERT_USART0_WRITE = 0x02000000,
  ASSERT_USART0_READ  = 0x03000000,
  ASSERT_COMMAND      = 0x04000000,
  ASSERT_PLAN         = 0x05000000,
  ASSERT_MAZE         = 0x06000000,
  ASSERT_MOTOR        = 0x07000000,
  ASSERT_PLATFORM     = 0x08000000,
  ASSERT_CONTROL      = 0x09000000,
  ASSERT_EXPLORE      = 0x0A000000,
  ASSERT_ADC          = 0x0B000000,
  ASSERT_TIMER        = 0x0C000000,
  ASSERT_ENCODERS     = 0x0D000000,
  ASSERT_SPEED        = 0x0E000000,
  ASSERT_WALLS        = 0x0F000000,
};

#if !defined(NOASSERT)
// assert asserts that the expression is true, and does not return if it is false.
#define assert(n, e)     \
  {                      \
    if (!(e)) {          \
      _assert_failed(n); \
    }                    \
  }
#else
#define assert(n, e)
#endif

// _assert_failed is a permanent error state.
void _assert_failed(uint32_t error);
