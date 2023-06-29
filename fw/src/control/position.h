#pragma once

#include <stdint.h>

// The measured robot position via encoder counts.
extern float position_measured_left;
extern float position_measured_right;

// The position setpoints via encoder count.
extern float position_setpoint_left;
extern float position_setpoint_right;

void position_init();
void position_update();

void position_enable();
void position_disable();

void position_set_left(float setpoint);
void position_set_right(float setpoint);

void position_set_pid_vars(float kp, float ki, float kd);
