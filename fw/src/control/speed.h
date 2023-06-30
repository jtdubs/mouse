#pragma once

#include <stdint.h>

#include "control/pid.h"

// The measured motor speeds via encoder period in microseconds.
extern float speed_measured_left;
extern float speed_measured_right;

// The speeds setpoints via encoder period in microsecond
extern float speed_setpoint_left;
extern float speed_setpoint_right;

extern pi_t speed_pi_left;
extern pi_t speed_pi_right;

void speed_init();
void speed_read();
void speed_update();

void speed_set(float left, float right);

void speed_set_pi_vars(float kp, float ki);
