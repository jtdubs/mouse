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

typedef void (*timer_callback_t)();

void timer_set_callback(timer_callback_t callback);

// timer_init initializes timer.
void timer_init();
