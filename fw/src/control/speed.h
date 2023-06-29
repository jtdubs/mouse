#pragma once

#include <stdint.h>

#include "control/pid.h"

// The measured motor speeds via encoder period in microseconds.
extern float speed_measured_left;
extern float speed_measured_right;

// The speeds setpoints via encoder period in microsecond
extern float speed_setpoint_left;
extern float speed_setpoint_right;

extern pid_t speed_pid_left;
extern pid_t speed_pid_right;

void speed_init();
void speed_update();

void speed_enable();
void speed_disable();

void speed_set_left(float setpoint);
void speed_set_right(float setpoint);

void speed_set_pid_vars(float kp, float ki, float kd);
