#pragma once

#include <stdint.h>

#include "modes/mode_error.h"

// Assert ID ranges.
#define ASSERT_MODE 0x10
#define ASSERT_USART0 0x20
#define ASSERT_COMMAND 0x30
#define ASSERT_BASE64 0x40

#if !defined(NOASSERT)
#define assert(n, e) ((e) ? (void)0 : mode_error(n))
#else
#define assert(n, e)
#endif
