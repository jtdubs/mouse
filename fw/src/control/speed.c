#include "speed.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/atomic.h>

#include "control/pid.h"
#include "platform/encoders.h"
#include "platform/motor.h"
#include "platform/rtc.h"

#define MAX_ENCODER_PERIOD 9000

// Motor speeds in RPM.
float speed_measured_left;
float speed_measured_right;

// Motor speed setpoints.
float speed_setpoint_left;
float speed_setpoint_right;

static bool  enabled;
static pid_t pid_left;
static pid_t pid_right;

float calculate_speed_left();
float calculate_speed_right();

void speed_init() {
  pid_left.min = 0;
  pid_left.max = 100;
  pid_left.kp  = 0.045;
  pid_left.ki  = 0.12;
  pid_left.kd  = 0.02;

  pid_right.min = 0;
  pid_right.max = 100;
  pid_right.kp  = 0.045;
  pid_right.ki  = 0.12;
  pid_right.kd  = 0.02;
}

void speed_update() {
  speed_measured_left  = calculate_speed_left();
  speed_measured_right = calculate_speed_right();

  if (enabled) {
    int8_t power_left = (int8_t)pid_update(&pid_left, fabsf(speed_setpoint_left), fabsf(speed_measured_left));
    if (power_left != 0) {
      power_left += 10;
    }

    int8_t power_right = (int8_t)pid_update(&pid_right, fabsf(speed_setpoint_right), fabsf(speed_measured_right));
    if (power_right != 0) {
      power_right += 10;
    }

    bool forward_left  = signbitf(speed_setpoint_left) == 0;
    bool forward_right = signbitf(speed_setpoint_right) == 0;

    motor_set_forward_left(forward_left);
    motor_set_power_left(power_left);
    motor_set_forward_right(forward_right);
    motor_set_power_right(power_right);
  }
}

void speed_enable() {
  enabled = true;
}

void speed_disable() {
  enabled = false;
}

// encoders_left_period returns the period of the left encoder in microseconds.
float calculate_speed_left() {
  uint32_t now = rtc_micros();
  uint32_t time0, time1;
  bool     forward;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    time0   = encoder_times_left[0];
    time1   = encoder_times_left[1];
    forward = encoder_forward_left;
  }
  if (now - time0 > MAX_ENCODER_PERIOD) {
    return 0;
  }
  float period = (float)(time0 - time1);
  float rpm    = (1000000.0 * 60.0 / 240.0) / period;
  return forward ? rpm : -rpm;
}

// encoders_right_period returns the period of the right encoder in microseconds.
float calculate_speed_right() {
  uint32_t now = rtc_micros();
  uint32_t time0, time1;
  bool     forward;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    time0   = encoder_times_right[0];
    time1   = encoder_times_right[1];
    forward = encoder_forward_right;
  }
  if ((now - time0) > MAX_ENCODER_PERIOD) {
    return 0;
  }
  float period = (float)(time0 - time1);
  float rpm    = (1000000.0 * 60.0 / 240.0) / period;
  return forward ? rpm : -rpm;
}

void speed_set_left(float setpoint) {
  speed_setpoint_left = setpoint;
}

void speed_set_right(float setpoint) {
  speed_setpoint_right = setpoint;
}
