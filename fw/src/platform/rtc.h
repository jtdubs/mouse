//
// System: platform
// Module: rtc
//
// Purpose:
// - Provides a microsecond-resolution clock (2us precision).
//
// Peripherals:
// - Owns the TIMER2 peripheral.
//
// Interrupts:
// - TIMER2_OVF when the timer overflows.
//
#pragma once

#include <stdint.h>

// rtc_init initializes the RTC.
void rtc_init();

// rtc_micros returns the number of microseconds since the RTC was initialized.
uint32_t rtc_micros();
