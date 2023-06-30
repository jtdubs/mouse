//
// System: platform
// Module: adc
//
// Purpose:
// - Samples the ADC channels
//
// Peripherals:
// - Owns the ADC peripheral.
//
// Interrupts:
// - ADC when a conversion is complete.
//
// Design:
// - When a sample is requested, all 6 channels are read in sequence.
//   This occurs in the background, with subsequent channels being
//   read whenever the previous conversion is complete.
// - It takes ~120us to read all 6 channels.
//
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

// adc_sample samples the ADC channels.
void adc_sample();
