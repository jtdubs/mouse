//
// System: platform
// Module: motor
//
// Purpose:
// - Provides functions for setting the motor speed and directio.
//
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "platform/pin.h"

// The power levels of each motor.
// Range: [-511, 511]
extern int16_t motor_power_left;
extern int16_t motor_power_right;

// motor_init initializes the motors.
void motor_init();

// Sets the power of the motors.
// Range: [-511, 511]
void motor_set(int16_t left, int16_t right);
