#pragma once

#if defined(BOARD_NANO)
#include "platform/nano/pin.h"
#elif defined(BOARD_NANOEVERY)
#include "platform/nanoevery/pin.h"
#else
#error "Unrecognized board"
#endif
