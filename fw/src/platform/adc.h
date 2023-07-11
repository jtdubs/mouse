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
  ADC_SENSOR_RIGHT    = 0,
  ADC_SENSOR_CENTER   = 1,
  ADC_SENSOR_LEFT     = 2,
  ADC_UNUSED_3        = 3,
  ADC_UNUSED_4        = 4,
  ADC_UNUSED_5        = 5,
  ADC_SELECTOR        = 6,
  ADC_BATTERY_VOLTAGE = 7,
  ADC_CHANNEL_COUNT   = 8,
} adc_channel_t;

// Raw 10-bit readings from ADC channels.
extern uint16_t adc_values[ADC_CHANNEL_COUNT];

// adc_init initializes the ADC.
void adc_init();

// adc_sample samples the ADC channels.
void adc_sample();
