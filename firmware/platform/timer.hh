//
// System: platform
// Module: timer
//
// Purpose:
// - Provides timer interrupts to drive the ADC and control loop.
//
// Peripherals:
// - Owns the TIMER0 peripheral.
//
// Interrupts:
// - TIMER0_COMPA every 5ms to run the control loop.
// - TIMER0_COMPB every 5ms (just before COMPA) to start ADC sampling.
//
#pragma once

namespace timer {

typedef void (*callback_t)();

// add_callback sets the callback function to be called on each timer expiration.
void add_callback(callback_t callback);

}  // namespace timer
