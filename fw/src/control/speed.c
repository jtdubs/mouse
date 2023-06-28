#include "speed.h"

#include <stdbool.h>
#include <stdint.h>

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

// Motor speeds measured via encoder period in microseconds.
int16_t speed_measured_left;
int16_t speed_measured_right;

// Motor speed setpoints.
int16_t speed_setpoint_left;
int16_t speed_setpoint_right;

int16_t calculate_speed_left();
int16_t calculate_speed_right();

void speed_init() {}

void speed_update() {
  speed_measured_left  = calculate_speed_left();
  speed_measured_right = calculate_speed_right();

  if (enabled) {
    motor_set_forward_left(speed_setpoint_left > 0);
    uint16_t left_magnitude = speed_setpoint_left > 0 ? speed_setpoint_left : -speed_setpoint_left;
    if (left_magnitude == 0 || left_magnitude > MAX_ENCODER_PERIOD) {
      motor_set_power_left(0);
    } else if (left_magnitude < MIN_ENCODER_PERIOD) {
      motor_set_power_left(MAX_MOTOR_POWER);
    } else {
      motor_set_power_left((MAX_ENCODER_PERIOD - left_magnitude) >> 6);
    }

    motor_set_forward_right(speed_setpoint_right > 0);
    uint16_t right_magnitude = speed_setpoint_right > 0 ? speed_setpoint_right : -speed_setpoint_right;
    if (right_magnitude == 0 || right_magnitude > MAX_ENCODER_PERIOD) {
      motor_set_power_right(0);
    } else if (right_magnitude < MIN_ENCODER_PERIOD) {
      motor_set_power_right(MAX_MOTOR_POWER);
    } else {
      motor_set_power_right((MAX_ENCODER_PERIOD - right_magnitude) >> 6);
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
int16_t calculate_speed_left() {
  uint32_t now = rtc_micros();
  if (now - encoder_times_left[0] > MAX_ENCODER_PERIOD) {
    return 0;
  }
  int16_t period = encoder_times_left[0] - encoder_times_left[1];
  return encoder_forward_left ? period : -period;
}

// encoders_right_period returns the period of the right encoder in microseconds.
int16_t calculate_speed_right() {
  uint32_t now = rtc_micros();
  if (now - encoder_times_right[0] > MAX_ENCODER_PERIOD) {
    return 0;
  }
  int16_t period = encoder_times_right[0] - encoder_times_right[1];
  return encoder_forward_right ? period : -period;
}

void speed_set_left(int16_t setpoint) {
  speed_setpoint_left = setpoint;
}

void speed_set_right(int16_t setpoint) {
  speed_setpoint_right = setpoint;
}
