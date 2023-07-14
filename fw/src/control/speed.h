//
// System: control
// Module: speed
//
// Purpose:
// - Used by other plans to drive the motors at a constant rotational speed.
//
// Design:
// - Uses characterizations of the motors to set a base PWM value for a given rotational speed.
// - Uses PID controllers to adjust the PWM values to compensate for errors.
//
#pragma once

#include <stdint.h>

#include "control/pid.h"

// speed_init initializes the speed module.
void speed_init();

// speed_update reads the current motor speeds.
void speed_update();

// speed_tick updates the motor PWM values.
void speed_tick();

// speed_set sets the motor speed setpoints.
void speed_set(float left, float right);

// speed_tune tunes the PID controllers.
void speed_tune(float kp, float ki, float kd, float alpha);

// speed_read reads the motor speeds.
void speed_read(float* left, float* right);

// speed_read_setpoints reads the motor speed setpoints.
void speed_read_setpoints(float* left, float* right);
