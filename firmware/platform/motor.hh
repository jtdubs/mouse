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

namespace motor {

// set sets the power levels of the motors.
// Range: [-511, 511]
void set(int16_t left, int16_t right);

// read reads the power levels of the motors.
// Range: [-511, 511]
void read(int16_t* left, int16_t* right);

}