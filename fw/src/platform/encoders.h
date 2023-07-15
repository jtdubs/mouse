#pragma once

#if defined(BOARD_NANO)
#include "platform/nano/encoders.h"
#elif defined(BOARD_NANOEVERY)
#include "platform/nanoevery/encoders.h"
#else
#error "Unrecognized board"
#endif
