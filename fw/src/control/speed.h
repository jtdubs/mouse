#pragma once

#include <stdint.h>

#include "control/pid.h"

// Measured motor speeds.
extern float speed_measured_left;   // rpms
extern float speed_measured_right;  // rpms

// Speed setpoints.
extern float speed_setpoint_left;   // rpms
extern float speed_setpoint_right;  // rpms

void speed_init();
void speed_read();
void speed_update();

void speed_set(float left, float right);

void speed_set_pi_coefficients(float kp, float ki, float alpha);
