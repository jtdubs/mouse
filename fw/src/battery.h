#pragma once

#include <stdbool.h>
#include <stdint.h>

extern uint8_t battery_voltage;  // from 0 to 10V

void battery_update();
