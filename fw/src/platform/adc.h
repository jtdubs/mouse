#pragma once

#include <stdint.h>

// Raw 10-bit readings from ADC channels.
extern uint16_t sensor_right;
extern uint16_t sensor_center;
extern uint16_t sensor_left;
extern uint16_t mode_selector;
extern uint16_t battery_voltage;

// adc_init initializes the ADC.
void adc_init();
