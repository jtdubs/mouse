#pragma once

#include <stdint.h>

#include "adc.h"

extern uint8_t battery_voltage;

inline void battery_update() {
  battery_voltage = (uint8_t)(adc_read(7) >> 2);
}
