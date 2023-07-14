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

// motor_init initializes the motors.
void motor_init();

// motor_set sets the power levels of the motors.
// Range: [-511, 511]
void motor_set(int16_t left, int16_t right);

// motor_read reads the power levels of the motors.
// Range: [-511, 511]
void motor_read(int16_t* left, int16_t* right);
