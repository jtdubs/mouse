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

void rtc_init();

uint32_t rtc_micros();
