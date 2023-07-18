#pragma once

#include <stdint.h>

#include "speed.hh"

namespace speed {

// Init initializes the speed module.
void Init();

// update reads the current motor speeds.
void Update();

// Tick updates the motor PWM values.
void Tick();

// read reads the motor speeds.
void Read(float& left, float& right);

// ReadSetpoints reads the motor speed setpoints.
void ReadSetpoints(float& left, float& right);

}  // namespace speed
