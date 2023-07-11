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
enum assert_range_t : uint8_t {
  ASSERT_MAIN         = 0x10,
  ASSERT_USART0_WRITE = 0x20,
  ASSERT_USART0_READ  = 0x30,
  ASSERT_COMMAND      = 0x40,
  ASSERT_PLAN         = 0x50,
  ASSERT_MAZE         = 0x60,
  ASSERT_MOTOR        = 0x70,
  ASSERT_PLATFORM     = 0x80,
  ASSERT_CONTROL      = 0x90,
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
void _assert_failed(uint8_t error);
