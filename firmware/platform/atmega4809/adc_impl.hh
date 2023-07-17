#pragma once

#include "firmware/platform/adc.hh"

namespace adc {

// next_channel defines the sequence in which channels are read.
constexpr channel_t NEXT_CHANNEL[CHANNEL_COUNT] = {
    SENSOR_FORWARD, SENSOR_LEFT, SELECTOR, SENSOR_RIGHT, SENSOR_RIGHT, SENSOR_RIGHT, BATTERY_VOLTAGE, SENSOR_RIGHT,
};

// first_channel is the first channel to be read.
constexpr channel_t FIRST_CHANNEL = SENSOR_RIGHT;

void init();

}  // namespace adc
