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

namespace adc {

enum class Channel : uint8_t {
  SensorRight,
  SensorForward,
  SensorLeft,
  Unused3,
  Unused4,
  Unused5,
  Selector,
  BatteryVoltage,
};

// sample samples the ADC channels.
void sample();

// read reads the value of an ADC channel.
uint16_t Read(Channel channel);

// read_sensors reads the value of an ADC channel.
void read_sensors(uint16_t& left, uint16_t& right, uint16_t& forward);

}  // namespace adc
