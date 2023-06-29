#include "position.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/atomic.h>

#include "control/pid.h"
#include "control/speed.h"
#include "platform/encoders.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "platform/rtc.h"

// Motor positions in RPM.
float position_measured_left;
float position_measured_right;

// Motor position setpoints.
float position_setpoint_left;
float position_setpoint_right;

static bool  position_enabled;
static pid_t position_pid_left;
static pid_t position_pid_right;

void position_init() {
  position_pid_left.min = -170;
  position_pid_left.max = 170;
  position_pid_left.kp  = 6;
  position_pid_left.ki  = 0.02;
  position_pid_left.kd  = 0.001;

  position_pid_right.min = -170;
  position_pid_right.max = 170;
  position_pid_right.kp  = 6;
  position_pid_right.ki  = 0.02;
  position_pid_right.kd  = 0.001;
}

#define ENC_TO_MM (32.0 * M_PI / 240.0)
#define MM_TO_ENC (240.0 / (32.0 * M_PI))

void position_update() {
  uint16_t l, r;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    l = encoder_left;
    r = encoder_right;
  }
  position_measured_left  = ((float)l) * ENC_TO_MM;
  position_measured_right = ((float)r) * ENC_TO_MM;

  if (position_enabled) {
    float speed_left = pid_update(&position_pid_left, position_setpoint_left, position_measured_left);
    if (fabsf(speed_left) < 0.5) {
      speed_left = 0;
    } else {
      speed_left += signbitf(speed_left) ? -29.5 : 29.5;
    }

    float speed_right = pid_update(&position_pid_right, position_setpoint_right, position_measured_right);
    if (fabsf(speed_right) < 0.5) {
      speed_right = 0;
    } else {
      speed_right += signbitf(speed_right) ? -29.5 : 29.5;
    }

    speed_set_left(speed_left);
    speed_set_right(speed_right);
  }

  speed_update();
}

void position_enable() {
  position_enabled = true;
  speed_enable();
}

void position_disable() {
  position_enabled = false;
  speed_disable();
}

void position_set_left(float setpoint) {
  position_setpoint_left = roundf(setpoint * MM_TO_ENC) * ENC_TO_MM;
}

void position_set_right(float setpoint) {
  position_setpoint_right = roundf(setpoint * MM_TO_ENC) * ENC_TO_MM;
}

void position_set_pid_vars(float kp, float ki, float kd) {
  position_pid_left.kp = kp;
  position_pid_left.ki = ki;
  position_pid_left.kd = kd;

  position_pid_right.kp = kp;
  position_pid_right.ki = ki;
  position_pid_right.kd = kd;
}
