#pragma once

#if defined(BOARD_NANO)
#include "platform/nano/timer.h"
#elif defined(BOARD_NANOEVERY)
#include "platform/nanoevery/timer.h"
#else
#error "Unrecognized board"
#endif
