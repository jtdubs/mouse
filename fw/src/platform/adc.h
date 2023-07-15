#pragma once

#if defined(BOARD_NANO)
#include "platform/nano/adc.h"
#elif defined(BOARD_NANOEVERY)
#include "platform/nanoevery/adc.h"
#else
#error "Unrecognized board"
#endif
