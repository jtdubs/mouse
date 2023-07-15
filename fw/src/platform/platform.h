#pragma once

#if defined(BOARD_NANO)
#include "platform/nano/platform.h"
#elif defined(BOARD_NANOEVERY)
#include "platform/nanoevery/platform.h"
#else
#error "Unrecognized board"
#endif
