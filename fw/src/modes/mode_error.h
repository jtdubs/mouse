#pragma once

#include <stdint.h>

extern uint8_t error_code;

void mode_error_tick();

void mode_error(uint8_t error);
