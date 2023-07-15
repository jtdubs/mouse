#pragma once

#if defined(BOARD_NANO)
#include "platform/nano/usart0.h"
#elif defined(BOARD_NANOEVERY)
#include "platform/nanoevery/usart0.h"
#else
#error "Unrecognized board"
#endif
