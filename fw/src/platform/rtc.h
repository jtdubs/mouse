#pragma once

#if defined(BOARD_NANO)
#include "platform/nano/rtc.h"
#elif defined(BOARD_NANOEVERY)
#include "platform/nanoevery/rtc.h"
#else
#error "Unrecognized board"
#endif
