#pragma once

#include <math.h>

typedef struct {
  float min, max;    // bounds for pid output
  float kp, ki, kd;  // pid constants
  float e, i;        // error and integral
} pid_t;

float pid_update(pid_t *pid, float sp, float pv);

void pid_reset(pid_t *pid);
