#include "pid.h"

#include <math.h>

float pid_update(pid_t *pid, float sp /* setpoint */, float pv /* process variable */) {
  float last_e = pid->e;
  pid->e       = sp - pv;
  float d      = last_e - pid->e;
  float i      = pid->i + pid->e;
  float out    = fmaf(pid->kp, pid->e, fmaf(pid->ki, i, pid->kd * d));
  if (out >= pid->min && out < pid->max) {
    // Only update the integrator if the output is within the min/max range.
    // This helps prevent the integrator from running away.
    pid->i = i;
  }
  return fmaxf(pid->min, fminf(pid->max, out));
}
