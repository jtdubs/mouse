//
// System: utils
// Module: pid
//
// Purpose:
// - Provide PID controllers and variants to the rest of the control system.
//
#pragma once

#include <math.h>

#include "math.hh"

namespace pid {

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

// update determines the manipulated value, given a setpoint and process variable.
inline float update(pid_t &pid, float sp /* setpoint */, float pv /* process variable */) {
  float p = sp - pv;           // proportional term is the error
  float i = pid.i + p;         // integral term is the sum of all errors
  float d = pv - pid.last_pv;  // derivative term is the change in the process variable

  // NOTE: The derivative term is classicaly the change in the error, but this is
  // sensitive to changes in the setpoint.  The change in the process variable, however,
  // is always continuous.  This is known as "setpoint weighting".

  pid.last_pv = pv;

  // out = kp * p + ki * i + kd * d
  float out = fmaf(pid.kp, p, fmaf(pid.ki, i, pid.kd * d));

  if (out >= pid.min && out < pid.max) {
    // Only update the integral if the output is within the min/max range.
    // This helps prevent the integral from running away, and is known as
    // "integral anti-windup".
    pid.i = i;
  }

  return math::clampf(out, pid.min, pid.max);
}

// reset resets the pid controller.
inline void reset(pid_t &pid) {
  pid.i       = 0;
  pid.last_pv = 0;
}

// update determines the manipulated value, given a setpoint and process variable.
inline float update(pi_t &pi, float sp /* setpoint */, float pv /* process variable */) {
  float p = sp - pv;   // proportional term is the error
  float i = pi.i + p;  // integral term is the sum of all errors

  // out = kp * p + ki * i
  float out = fmaf(pi.kp, p, pi.ki * i);

  if (out >= pi.min && out < pi.max) {
    // Only update the integral if the output is within the min/max range.
    // This helps prevent the integral from running away, and is known as
    // "integral anti-windup".
    pi.i = i;
  }

  return math::clampf(out, pi.min, pi.max);
}

// reset resets the pi controller.
inline void reset(pi_t &pi) {
  pi.i = 0;
}

// update determines the manipulated value, given a setpoint and process variable.
inline float update(pd_t &pd, float sp /* setpoint */, float pv /* process variable */) {
  float p = sp - pv;          // proportional term is the error
  float d = pv - pd.last_pv;  // derivative term is the change in the process variable

  // NOTE: The derivative term is classicaly the change in the error, but this is
  // sensitive to changes in the setpoint.  The change in the process variable, however,
  // is always continuous.  This is known as "setpoint weighting".

  pd.last_pv = pv;

  // out = kp * p + kd * d
  float out = fmaf(pd.kp, p, pd.kd * d);

  return math::clampf(out, pd.min, pd.max);
}

// reset resets the pd controller.
inline void reset(pd_t &pd) {
  pd.last_pv = 0;
}

// update determines the manipulated value, given a setpoint and process variable.
inline float update(p_t &p, float sp /* setpoint */, float pv /* process variable */) {
  // out = kp * p
  float out = p.kp * (sp - pv);

  return math::clampf(out, p.min, p.max);
}

// reset resets the p controller.
inline void reset([[maybe_unused]] p_t &p) {}

}  // namespace pid
