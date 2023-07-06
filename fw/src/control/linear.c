#include "linear.h"

#include <stdbool.h>
#include <util/atomic.h>

#include "control/config.h"
#include "control/position.h"
#include "control/sensor_cal.h"
#include "control/speed.h"
#include "platform/adc.h"
#include "platform/pin.h"
#include "utils/math.h"

float linear_start_distance;   // mm
float linear_target_distance;  // mm
float linear_target_speed;     // mm/s

void linear_init() {}

void linear_start(float distance /* mm */, bool stop) {
  linear_start_distance  = position_distance;             // mm
  linear_target_distance = position_distance + distance;  // mm
  linear_target_speed    = stop ? 0.0 : SPEED_CRUISE;     // mm/s

  pin_set(IR_LEDS);
}

int16_t wall_error_left;
int16_t wall_error_right;
bool    left_wall_present;
bool    right_wall_present;

float calculate_wall_error() {
  uint16_t left  = adc_values[ADC_SENSOR_LEFT];
  uint16_t right = adc_values[ADC_SENSOR_RIGHT];

  wall_error_left  = left - sensor_threshold_left;
  wall_error_right = sensor_threshold_right - right;

  left_wall_present  = (left >= (sensor_threshold_left >> 1));
  right_wall_present = (right >= (sensor_threshold_right >> 1));

  pin_set2(LED_LEFT, left_wall_present);
  pin_set2(LED_RIGHT, right_wall_present);

  if (!left_wall_present && !right_wall_present) {
    return 0;
  } else if (left_wall_present && !right_wall_present) {
    return wall_error_left * 2.0f;
  } else if (!left_wall_present && right_wall_present) {
    return wall_error_right * 2.0f;
  } else {
    return (float)(wall_error_left + wall_error_right);
  }
}

bool linear_update() {
  uint16_t center = adc_values[ADC_SENSOR_CENTER];

  // Emergency stop if too close to a wall.
  if (center > 380) {
    pin_clear(IR_LEDS);
    speed_set(0, 0);
    return true;
  }

  // If we are there, then we are done.
  if (position_distance >= linear_target_distance) {
    float rpm = SPEED_TO_RPM(linear_target_speed);
    pin_clear(IR_LEDS);
    speed_set(rpm, rpm);
    return true;
  }

  float current_speed    = RPM_TO_SPEED((speed_measured_left + speed_measured_right) / 2.0f);  // mm/s
  float current_setpoint = RPM_TO_SPEED((speed_setpoint_left + speed_setpoint_right) / 2.0f);  // mm/s

  // Compute the braking distance.
  static float braking_accel = 0;  // mm/s^2
  if (current_speed > linear_target_speed) {
    float dV      = linear_target_speed - current_speed;                      // mm/s
    float dX      = linear_target_distance - position_distance;               // mm
    braking_accel = ((2.0f * current_speed * dV) + (dV * dV)) / (2.0f * dX);  // mm/s^2
  }

  // Determine the acceleration.
  float accel = 0;  // mm/s^2
  if (braking_accel <= -ACCEL_DEFAULT) {
    accel = braking_accel;
  } else if (current_speed < SPEED_CRUISE) {
    accel = ACCEL_DEFAULT;
  }

  // Calculate the new setpoint.
  // Starting with the current setpoint.
  float left_speed  = current_setpoint;
  float right_speed = current_setpoint;
  // Add the acceleration.
  left_speed  += (accel * CONTROL_PERIOD);  // mm/s
  right_speed += (accel * CONTROL_PERIOD);  // mm/s
  // Adjust for the wall error.
  float wall_error  = calculate_wall_error();
  left_speed       += (wall_error * 0.1);
  right_speed      -= (wall_error * 0.1);
  // Adjust for the wheel bias.
  left_speed  *= (1.0 - WHEEL_BIAS);
  right_speed *= (1.0 + WHEEL_BIAS);
  // Convert to RPMs.
  left_speed  = CLAMP_RPM(SPEED_TO_RPM(left_speed));
  right_speed = CLAMP_RPM(SPEED_TO_RPM(right_speed));

  speed_set(left_speed, right_speed);
  speed_update();
  return false;
}
