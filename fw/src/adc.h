#pragma once

#include <assert.h>
#include <stdint.h>

extern uint16_t sensor_right;
extern uint16_t sensor_center;
extern uint16_t sensor_left;
extern uint16_t mode_selector;
extern uint16_t battery_voltage;

void adc_init();
