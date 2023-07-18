#pragma once

#include <stdint.h>

#include "speed.hh"

namespace speed {

// init initializes the speed module.
void init();

// update reads the current motor speeds.
void update();

// tick updates the motor PWM values.
void tick();

// read reads the motor speeds.
void read(float& left, float& right);

// read_setpoints reads the motor speed setpoints.
void read_setpoints(float& left, float& right);

}  // namespace speed
