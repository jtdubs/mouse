#include "pid.h"

#include <math.h>

// clampf constrains x to the range [min, max].
inline float clampf(float x, float min, float max) {
  return fmaxf(min, fminf(max, x));
}

// pid_update determines the manipulated value, given a setpoint and process variable.
float pid_update(pid_t *pid, float sp /* setpoint */, float pv /* process variable */) {
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
void pid_reset(pid_t *pid) {
  pid->i       = 0;
  pid->last_pv = 0;
}

// pi_update determines the manipulated value, given a setpoint and process variable.
float pi_update(pi_t *pi, float sp /* setpoint */, float pv /* process variable */) {
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
void pi_reset(pi_t *pi) {
  pi->i = 0;
}
