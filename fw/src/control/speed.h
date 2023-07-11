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

// Measured motor speeds.
extern float speed_measured_left;   // rpms
extern float speed_measured_right;  // rpms

// Speed setpoints.
extern float speed_setpoint_left;   // rpms
extern float speed_setpoint_right;  // rpms

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
