#include "pid.h"

#include <math.h>

float pid_update(pid_t *pid, float sp /* setpoint */, float pv /* process variable */) {
  float p = sp - pv;
  float i = pid->i + (pid->ki * p);
  float d = pv - pid->last_pv;

  pid->last_pv = pv;

  float out = fmaf(pid->kp, p, fmaf(pid->kd, d, i));

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
