#include "speed.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/atomic.h>

#include "control/pid.h"
#include "platform/encoders.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "platform/rtc.h"

#define MAX_ENCODER_PERIOD 9000
#define MIN_SPEED 5.0

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
  pid_left.max = 200;
  pid_left.kp  = 0.08;
  pid_left.ki  = 0.13;
  pid_left.kd  = 0.002;

  pid_right.min = 0;
  pid_right.max = 200;
  pid_right.kp  = 0.08;
  pid_right.ki  = 0.13;
  pid_right.kd  = 0.002;

  speed_setpoint_left  = 90.0;
  speed_setpoint_right = 90.0;
}

void speed_update() {
  speed_measured_left  = calculate_speed_left();
  speed_measured_right = calculate_speed_right();

  if (enabled) {
    uint8_t power_left;
    if (fabsf(speed_setpoint_left) < MIN_SPEED) {
      pid_reset(&pid_left);
      power_left = 0;
    } else {
      power_left = (uint8_t)pid_update(&pid_left, fabsf(speed_setpoint_left), fabsf(speed_measured_left));
      if (power_left != 0) {
        power_left += 26;
      }
    }

    uint8_t power_right;
    if (fabsf(speed_setpoint_right) < MIN_SPEED) {
      pid_reset(&pid_right);
      power_right = 0;
    } else {
      power_right = (uint8_t)pid_update(&pid_right, fabsf(speed_setpoint_right), fabsf(speed_measured_right));
      if (power_right != 0) {
        power_right += 26;
      }
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
  uint32_t now, time0, time1;
  bool     forward;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    time0   = encoder_times_left[0];
    time1   = encoder_times_left[1];
    forward = encoder_forward_left;
    now     = rtc_micros();
  }
  if ((now - time0) > MAX_ENCODER_PERIOD) {
    pin_toggle(PROBE_1);
    return 0;
  }
  uint32_t dt = time0 - time1;
  if (dt == 0) {
    return 0;
  }
  float period = (float)(time0 - time1);
  float rpm    = (1000000.0 * 60.0 / 240.0) / period;
  return forward ? rpm : -rpm;
}

// encoders_right_period returns the period of the right encoder in microseconds.
float calculate_speed_right() {
  uint32_t now, time0, time1;
  bool     forward;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    time0   = encoder_times_right[0];
    time1   = encoder_times_right[1];
    forward = encoder_forward_right;
    now     = rtc_micros();
  }
  if ((now - time0) > MAX_ENCODER_PERIOD) {
    pin_toggle(PROBE_2);
    return 0;
  }
  uint32_t dt = time0 - time1;
  if (dt == 0) {
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
