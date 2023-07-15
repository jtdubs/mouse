#pragma once

#if defined(BOARD_NANO)
#include "platform/nano/motor.h"
#elif defined(BOARD_NANOEVERY)
#include "platform/nanoevery/motor.h"
#else
#error "Unrecognized board"
#endif
