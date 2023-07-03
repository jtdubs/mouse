#include "linear.h"

#include <stdbool.h>
#include <util/atomic.h>

#include "control/config.h"
#include "control/position.h"
#include "control/speed.h"
#include "utils/math.h"

float linear_start_distance;   // mm
float linear_target_distance;  // mm
float linear_target_speed;     // mm/s

void linear_init() {}

void linear_start(float distance /* mm */, bool stop) {
  linear_start_distance  = position_distance;             // mm
  linear_target_distance = position_distance + distance;  // mm
  linear_target_speed    = stop ? 0.0 : SPEED_CRUISE;     // mm/s
}

bool linear_update() {
  // If we are there, then we are done.
  if (position_distance >= linear_target_distance) {
    float rpm = SPEED_TO_RPM(linear_target_speed);
    speed_set(rpm, rpm);
    return true;
  }

  float current_speed    = RPM_TO_SPEED((speed_measured_left + speed_measured_right) / 2.0);  // mm/s
  float current_setpoint = RPM_TO_SPEED((speed_setpoint_left + speed_setpoint_right) / 2.0);  // mm/s

  // Compute the braking distance.
  static float braking_accel = 0;  // mm/s^2
  if (current_speed > linear_target_speed) {
    float dV      = linear_target_speed - current_speed;                    // mm/s
    float dX      = linear_target_distance - position_distance;             // mm
    braking_accel = ((2.0 * current_speed * dV) + (dV * dV)) / (2.0 * dX);  // mm/s^2
  }

  float accel = 0;  // mm/s^2
  if (braking_accel <= -ACCEL_DEFAULT) {
    accel = braking_accel;
  } else if (current_speed < SPEED_CRUISE) {
    accel = ACCEL_DEFAULT;
  }

  float new_speed = current_setpoint + (accel * CONTROL_PERIOD);  // mm/s

  float left_speed  = CLAMP_RPM(SPEED_TO_RPM(new_speed * (1.0 - WHEEL_BIAS)));  // RPMs
  float right_speed = CLAMP_RPM(SPEED_TO_RPM(new_speed * (1.0 + WHEEL_BIAS)));  // RPMs

  speed_set(left_speed, right_speed);
  speed_update();
  return false;
}
