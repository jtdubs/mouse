#include "speed.h"

#include <stdbool.h>
#include <stdint.h>

#include "platform/encoders.h"
#include "platform/rtc.h"

// At the lowest achievable speed, the encoder will trigger every 7.5ms.
#define MAX_ENCODER_PERIOD 75000

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
    // TODO: Implement PID control.
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
