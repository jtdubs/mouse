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
  ASSERT_MAIN         = 0x10000000,
  ASSERT_USART0_WRITE = 0x20000000,
  ASSERT_USART0_READ  = 0x30000000,
  ASSERT_COMMAND      = 0x40000000,
  ASSERT_PLAN         = 0x50000000,
  ASSERT_MAZE         = 0x60000000,
  ASSERT_MOTOR        = 0x70000000,
  ASSERT_PLATFORM     = 0x80000000,
  ASSERT_CONTROL      = 0x90000000,
  ASSERT_EXPLORE      = 0xA0000000,
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
