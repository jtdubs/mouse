#pragma once

#if defined(BOARD_NANO)
#include "platform/nano/selector.h"
#elif defined(BOARD_NANOEVERY)
#include "platform/nanoevery/selector.h"
#else
#error "Unrecognized board"
#endif
