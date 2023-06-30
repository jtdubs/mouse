#pragma once

#include <stdint.h>

void _assert_failed(uint8_t error);

// Assert ID ranges.
#define ASSERT_MODE 0x10
#define ASSERT_USART0_WRITE 0x20
#define ASSERT_USART0_READ 0x30
#define ASSERT_COMMAND 0x40
#define ASSERT_BASE64 0x50

#if !defined(NOASSERT)
#define assert(n, e)     \
  {                      \
    if (!(e)) {          \
      _assert_failed(n); \
      return;            \
    }                    \
  }
#else
#define assert(n, e)
#endif
