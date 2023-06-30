#pragma once

#include <math.h>

// PID controller for a process variable.
typedef struct {
  float min, max;    // bounds for pid output
  float kp, ki, kd;  // pid constants
  float i;           // integral
  float last_pv;     // last process variable
} pid_t;

// pid_update determines the manipulated value, given a setpoint and process variable.
float pid_update(pid_t *pid, float sp, float pv);

// pid_reset resets the pid controller.
void pid_reset(pid_t *pid);

// PI controller for a process variable.
typedef struct {
  float min, max;  // bounds for pid output
  float kp, ki;    // pi constants
  float i;         // integral
} pi_t;

// pi_update determines the manipulated value, given a setpoint and process variable.
float pi_update(pi_t *pi, float sp, float pv);

// pi_reset resets the pi controller.
void pi_reset(pi_t *pi);
