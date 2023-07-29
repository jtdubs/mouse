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

namespace mouse::platform::timer {

typedef void (*callback_t)();

// AddCallback sets the callback function to be called on each timer expiration.
void AddCallback(callback_t callback);

}  // namespace mouse::platform::timer
