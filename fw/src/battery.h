#pragma once

#include <stdbool.h>
#include <stdint.h>

extern uint16_t battery_voltage;  // in mV

void battery_update();
