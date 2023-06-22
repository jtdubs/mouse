#pragma once

#include <stdint.h>

#include "mode_error.h"

#if !defined(NOASSERT)
#define assert(e) ((e) ? (void)0 : mode_error(__LINE__))
#if defined(DEBUG)
#define assert_debug(e) ((e) ? (void)0 : mode_error(__LINE__))
#else
#define assert_debug(e)
#endif
#else
#define assert(e)
#define assert_debug(e)
#endif
