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

#if defined(NO_ASSERT)
#define assert(m, n, e)
#else
// assert asserts that the expression is true, and does not return if it is false.
#define assert(m, n, e)     \
  {                         \
    if (!(e)) {             \
      assert::Failed(m, n); \
    }                       \
  }
#endif

namespace assert {

// range_t defines the ranges of assert codes available to each module.
enum class Module : uint8_t {
  Main,
  Usart0Write,
  Usart0Read,
  Command,
  Plan,
  Maze,
  Motor,
  Platform,
  Control,
  Explore,
  Adc,
  Timer,
  Encoders,
  Speed,
  Walls,
  Sensor,
  Position,
  Linear,
  Dequeue,
};

// failed is a permanent error state.
void Failed(Module m, uint8_t n);

}  // namespace assert
