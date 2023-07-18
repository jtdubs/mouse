#pragma once

#include "firmware/platform/adc.hh"

namespace adc {

// next_channel defines the sequence in which channels are read.
constexpr Channel NEXT_CHANNEL[8] = {
    Channel::SensorForward, Channel::SensorLeft,  Channel::Selector,       Channel::SensorRight,
    Channel::SensorRight,   Channel::SensorRight, Channel::BatteryVoltage, Channel::SensorRight,
};

// first_channel is the first channel to be read.
constexpr Channel FIRST_CHANNEL = Channel::SensorRight;

void init();

}  // namespace adc
