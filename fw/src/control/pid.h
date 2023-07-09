#pragma once

#include <math.h>

#include "utils/math.h"

// PID controller for a process variable.
typedef struct {
  float min, max;    // bounds for pid output
  float kp, ki, kd;  // pid constants
  float i;           // integral
  float last_pv;     // last process variable
} pid_t;

// PI controller for a process variable.
typedef struct {
  float min, max;  // bounds for pi output
  float kp, ki;    // pi constants
  float i;         // integral
} pi_t;

// PD controller for a process variable.
typedef struct {
  float min, max;  // bounds for pd output
  float kp, kd;    // pd constants
  float last_pv;   // last process variable
} pd_t;

// P controller for a process variable.
typedef struct {
  float min, max;  // bounds for pid output
  float kp;        // pid constants
} p_t;

// pid_update determines the manipulated value, given a setpoint and process variable.
inline float pid_update(pid_t *pid, float sp /* setpoint */, float pv /* process variable */) {
  float p = sp - pv;            // proportional term is the error
  float i = pid->i + p;         // integral term is the sum of all errors
  float d = pv - pid->last_pv;  // derivative term is the change in the process variable

  // NOTE: The derivative term is classicaly the change in the error, but this is
  // sensitive to changes in the setpoint.  The change in the process variable, however,
  // is always continuous.  This is known as "setpoint weighting".

  pid->last_pv = pv;

  // out = kp * p + ki * i + kd * d
  float out = fmaf(pid->kp, p, fmaf(pid->ki, i, pid->kd * d));

  if (out >= pid->min && out < pid->max) {
    // Only update the integral if the output is within the min/max range.
    // This helps prevent the integral from running away, and is known as
    // "integral anti-windup".
    pid->i = i;
  }

  return clampf(out, pid->min, pid->max);
}

// pid_reset resets the pid controller.
inline void pid_reset(pid_t *pid) {
  pid->i       = 0;
  pid->last_pv = 0;
}

// pi_update determines the manipulated value, given a setpoint and process variable.
inline float pi_update(pi_t *pi, float sp /* setpoint */, float pv /* process variable */) {
  float p = sp - pv;    // proportional term is the error
  float i = pi->i + p;  // integral term is the sum of all errors

  // out = kp * p + ki * i
  float out = fmaf(pi->kp, p, pi->ki * i);

  if (out >= pi->min && out < pi->max) {
    // Only update the integral if the output is within the min/max range.
    // This helps prevent the integral from running away, and is known as
    // "integral anti-windup".
    pi->i = i;
  }

  return clampf(out, pi->min, pi->max);
}

// pi_reset resets the pi controller.
inline void pi_reset(pi_t *pi) {
  pi->i = 0;
}

// pd_update determines the manipulated value, given a setpoint and process variable.
inline float pd_update(pd_t *pd, float sp /* setpoint */, float pv /* process variable */) {
  float p = sp - pv;           // proportional term is the error
  float d = pv - pd->last_pv;  // derivative term is the change in the process variable

  // NOTE: The derivative term is classicaly the change in the error, but this is
  // sensitive to changes in the setpoint.  The change in the process variable, however,
  // is always continuous.  This is known as "setpoint weighting".

  pd->last_pv = pv;

  // out = kp * p + kd * d
  float out = fmaf(pd->kp, p, pd->kd * d);

  return clampf(out, pd->min, pd->max);
}

// pd_reset resets the pd controller.
inline void pd_reset(pd_t *pd) {
  pd->last_pv = 0;
}

// p_update determines the manipulated value, given a setpoint and process variable.
inline float p_update(p_t *p, float sp /* setpoint */, float pv /* process variable */) {
  // out = kp * p
  float out = p->kp * (sp - pv);

  return clampf(out, p->min, p->max);
}

// p_reset resets the p controller.
inline void p_reset([[maybe_unused]] p_t *p) {}
