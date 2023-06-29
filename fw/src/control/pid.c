#include "pid.h"

#include <math.h>

float pid_update(pid_t *pid, float sp /* setpoint */, float pv /* process variable */) {
  pid->e       = sp - pv;
  float d      = pid->last_pv - pv;  // maybe backwards??
  pid->last_pv = pv;

  float i   = pid->i + (pid->ki * pid->e);  // + (pid->kp * e); ??
  float out = fmaf(pid->kp, pid->e, fmaf(pid->kd, d, i));
  if (out >= pid->min && out < pid->max) {
    // Only update the integrator if the output is within the min/max range.
    // This helps prevent the integrator from running away.
    pid->i = i;
  }

  return fmaxf(pid->min, fminf(pid->max, out));
}

void pid_reset(pid_t *pid) {
  pid->e       = 0;
  pid->i       = 0;
  pid->last_pv = 0;
}
