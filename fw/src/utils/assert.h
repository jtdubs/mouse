#pragma once

#include <stdint.h>

#include "modes/mode_error.h"

// Assert ID ranges.
#define ASSERT_MODE 0x10
#define ASSERT_USART0_WRITE 0x20
#define ASSERT_USART0_READ 0x30
#define ASSERT_COMMAND 0x40
#define ASSERT_BASE64 0x50

#if !defined(NOASSERT)
#define assert(n, e) \
  {                  \
    if (!(e)) {      \
      mode_error(n); \
      return;        \
    }                \
  }
#else
#define assert(n, e)
#endif
