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
#define MIN_SPEED 30.0

// Motor speeds in RPM.
float speed_measured_left;
float speed_measured_right;

// Motor speed setpoints.
float speed_setpoint_left;
float speed_setpoint_right;

static bool  speed_enabled;
static pid_t speed_pid_left;
static pid_t speed_pid_right;

static float calculate_speed_left();
static float calculate_speed_right();

void speed_init() {
  speed_pid_left.min = 0;
  speed_pid_left.max = 200;
  speed_pid_left.kp  = 0.08;
  speed_pid_left.ki  = 0.15;
  speed_pid_left.kd  = 0.005;

  speed_pid_right.min = 0;
  speed_pid_right.max = 200;
  speed_pid_right.kp  = 0.08;
  speed_pid_right.ki  = 0.15;
  speed_pid_right.kd  = 0.005;
}

void speed_update() {
  speed_measured_left  = calculate_speed_left();
  speed_measured_right = calculate_speed_right();

  if (speed_enabled) {
    uint8_t power_left;
    if (fabsf(speed_setpoint_left) < MIN_SPEED) {
      pid_reset(&speed_pid_left);
      power_left = 0;
    } else {
      power_left = (uint8_t)pid_update(&speed_pid_left, fabsf(speed_setpoint_left), fabsf(speed_measured_left));
      if (power_left != 0) {
        power_left += 26;
      }
    }

    uint8_t power_right;
    if (fabsf(speed_setpoint_right) < MIN_SPEED) {
      pid_reset(&speed_pid_right);
      power_right = 0;
    } else {
      power_right = (uint8_t)pid_update(&speed_pid_right, fabsf(speed_setpoint_right), fabsf(speed_measured_right));
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
  speed_enabled = true;
}

void speed_disable() {
  speed_enabled = false;
}

// encoders_left_period returns the period of the left encoder in microseconds.
static float calculate_speed_left() {
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
static float calculate_speed_right() {
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
