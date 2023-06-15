#pragma once

#include <stdbool.h>
#include <stdint.h>

extern uint16_t battery_voltage;  // in mV

void battery_init();
void battery_update();
