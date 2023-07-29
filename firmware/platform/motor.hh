//
// System: platform
// Module: motor
//
// Purpose:
// - Provides functions for setting the motor speed and directio.
//
#pragma once

#include <stdint.h>

#include "pin.hh"

namespace mouse::platform::motor {

// set sets the power levels of the motors.
// Range: [-511, 511]
void Set(int16_t left, int16_t right);

// read reads the power levels of the motors.
// Range: [-511, 511]
void Read(int16_t& left, int16_t& right);

}  // namespace mouse::platform::motor
