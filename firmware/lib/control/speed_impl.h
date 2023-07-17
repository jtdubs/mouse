#pragma once

#include <stdint.h>

#include "speed.h"

// speed_init initializes the speed module.
void speed_init();

// speed_update reads the current motor speeds.
void speed_update();

// speed_tick updates the motor PWM values.
void speed_tick();

// speed_read reads the motor speeds.
void speed_read(float* left, float* right);

// speed_read_setpoints reads the motor speed setpoints.
void speed_read_setpoints(float* left, float* right);
