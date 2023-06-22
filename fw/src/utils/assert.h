#pragma once

#include <stdint.h>

#include "modes/mode_error.h"

// TODO: assert statements are all uniquely identified so that
// the error_mode's blink codes can indicate which assert failed.

#if !defined(NOASSERT)
#define assert(e) ((e) ? (void)0 : mode_error(__LINE__))
#else
#define assert(e)
#endif
