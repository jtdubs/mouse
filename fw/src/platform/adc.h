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

typedef enum : uint8_t {
  ADC_SENSOR_RIGHT,
  ADC_SENSOR_CENTER,
  ADC_SENSOR_LEFT,
  ADC_UNUSED_3,
  ADC_UNUSED_4,
  ADC_UNUSED_5,
  ADC_SELECTOR,
  ADC_BATTERY_VOLTAGE,
  ADC_CHANNEL_COUNT,
} adc_channel_t;

// Raw 10-bit readings from ADC channels.
extern uint16_t adc_values[ADC_CHANNEL_COUNT];

// adc_init initializes the ADC.
void adc_init();

// adc_sample samples the ADC channels.
void adc_sample();
