#pragma once

#include <math.h>

typedef struct {
  float min, max;    // bounds for pid output
  float kp, ki, kd;  // pid constants
  float i;           // integral
  float last_pv;     // last process variable
} pid_t;

float pid_update(pid_t *pid, float sp, float pv);

void pid_reset(pid_t *pid);

typedef struct {
  float min, max;  // bounds for pid output
  float kp, ki;    // pid constants
  float i;         // integral
  float last_pv;   // last process variable
} pi_t;

float pi_update(pi_t *pi, float sp, float pv);

void pi_reset(pi_t *pi);
