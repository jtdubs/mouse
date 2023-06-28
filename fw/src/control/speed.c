#include "speed.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/atomic.h>

#include "platform/encoders.h"
#include "platform/motor.h"
#include "platform/rtc.h"

// The range of achievable encoder periods.
#define MIN_ENCODER_PERIOD 1000
#define MAX_ENCODER_PERIOD 7500

// The range of reasonable motor power levels.
#define MIN_MOTOR_POWER 12
#define MAX_MOTOR_POWER 100

static bool enabled;

// Motor speeds in RPM.
float speed_measured_left;
float speed_measured_right;

// Motor speed setpoints.
float speed_setpoint_left;
float speed_setpoint_right;

// Error values
float speed_error_left;
float speed_error_right;

// Integrator values
float speed_integrator_left;
float speed_integrator_right;

float calculate_speed_left();
float calculate_speed_right();

void speed_init() {}

void speed_update() {
  const float kp = 0.05, ki = 0.12, kd = 0.02;

  speed_measured_left  = calculate_speed_left();
  speed_measured_right = calculate_speed_right();

  if (enabled) {
    float last_error_left = speed_error_left;
    speed_error_left      = speed_setpoint_left - speed_measured_left;
    float integrator_left = speed_integrator_left + speed_error_left;
    float power_left = (kp * speed_error_left) + (ki * integrator_left) + (kd * (last_error_left - speed_error_left));
    if (power_left > 0.0 && power_left < 100.0) {
      speed_integrator_left = integrator_left;
    } else if (power_left < 0) {
      power_left = 0.0;
    } else if (power_left > 100.0) {
      power_left = 100.0;
    }
    if (power_left == 0.0) {
      motor_set_power_left((uint8_t)power_left);
    } else {
      motor_set_power_left(((uint8_t)power_left) + 12);
    }

    float last_error_right = speed_error_right;
    speed_error_right      = speed_setpoint_right - speed_measured_right;
    float integrator_right = speed_integrator_right + speed_error_right;
    float power_right =
        (kp * speed_error_right) + (ki * integrator_right) + (kd * (last_error_right - speed_error_right));
    if (power_right > 0.0 && power_right < 100.0) {
      speed_integrator_right = integrator_right;
    } else if (power_right < 0) {
      power_right = 0.0;
    } else if (power_right > 100.0) {
      power_right = 100.0;
    }
    if (power_right == 0.0) {
      motor_set_power_right((uint8_t)power_right);
    } else {
      motor_set_power_right(((uint8_t)power_right) + 12);
    }
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
