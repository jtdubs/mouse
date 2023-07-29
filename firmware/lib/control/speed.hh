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

namespace mouse::control::speed {

// set sets the motor speed setpoints.
void Set(float left, float right);

// tune tunes the PID controllers.
void TunePID(float kp, float ki, float kd, float alpha);

}  // namespace mouse::control::speed
