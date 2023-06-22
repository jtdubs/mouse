#pragma once

#include <stdint.h>

// error_code is the error code to be displayed.
extern uint8_t error_code;

// mode_error_tick is the error mode's tick function.
void mode_error_tick();

// mode_error sets the error code, and enters the error mode.
void mode_error(uint8_t error);
