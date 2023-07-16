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

// platform_init initializes the platform system.
void platform_init();

// platform_report returns a report of the platform state.
uint8_t platform_report(uint8_t *buffer, uint8_t len);
