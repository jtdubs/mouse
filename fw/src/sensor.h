#pragma once

#include <stdint.h>

#include "adc.h"

extern uint16_t sensor_left;
extern uint16_t sensor_center;
extern uint16_t sensor_right;

inline void sensor_update() {
  sensor_left   = adc_read(2);
  sensor_center = adc_read(1);
  sensor_right  = adc_read(0);
}
