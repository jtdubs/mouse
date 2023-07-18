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

#include "adc.hh"
#include "encoders.hh"
#include "motor.hh"
#include "pin.hh"
#include "rtc.hh"
#include "selector.hh"
#include "timer.hh"
#include "usart0.hh"

namespace platform {

// init initializes the platform system.
void init();

// report returns a report of the platform state.
uint8_t report(uint8_t *buffer, uint8_t len);

}  // namespace platform
