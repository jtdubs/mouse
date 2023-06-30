#include "pid.h"

#include <math.h>

float pid_update(pid_t *pid, float sp /* setpoint */, float pv /* process variable */) {
  float p = sp - pv;
  float i = pid->i + p;
  float d = pv - pid->last_pv;

  pid->last_pv = pv;

  float out = fmaf(pid->kp, p, fmaf(pid->ki, i, pid->kd * d));

  if (out >= pid->min && out < pid->max) {
    // Only update the integrator if the output is within the min/max range.
    // This helps prevent the integrator from running away.
    pid->i = i;
  }

  return fmaxf(pid->min, fminf(pid->max, out));
}

void pid_reset(pid_t *pid) {
  pid->i       = 0;
  pid->last_pv = 0;
}

float pi_update(pi_t *pi, float sp /* setpoint */, float pv /* process variable */) {
  float p = sp - pv;
  float i = pi->i + p;

  pi->last_pv = pv;

  float out = fmaf(pi->kp, p, pi->ki * i);

  if (out >= pi->min && out < pi->max) {
    // Only update the integrator if the output is within the min/max range.
    // This helps prevent the integrator from running away.
    pi->i = i;
  }

  return fmaxf(pi->min, fminf(pi->max, out));
}

void pi_reset(pi_t *pi) {
  pi->i       = 0;
  pi->last_pv = 0;
}
