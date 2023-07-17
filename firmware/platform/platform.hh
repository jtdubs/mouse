//
// System: platform
// Module: platform
//
// Purpose:
// - Configures all the board's peripherals.
// - Owns direction interaction with the peripherals.
//
#pragma once

#include <stdint.h>

namespace platform {

// init initializes the platform system.
void init();

// report returns a report of the platform state.
uint8_t report(uint8_t *buffer, uint8_t len);

}  // namespace platform
