#pragma once

#include <stdint.h>

// Raw 10-bit readings from ADC channels.
extern uint16_t adc_sensor_right;
extern uint16_t adc_sensor_center;
extern uint16_t adc_sensor_left;
extern uint16_t adc_selector;
extern uint16_t adc_battery_voltage;

// adc_init initializes the ADC.
void adc_init();
