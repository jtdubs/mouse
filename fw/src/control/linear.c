#include "linear.h"

#include <stdbool.h>

#include "control/config.h"
#include "control/position.h"
#include "control/speed.h"
#include "utils/math.h"

float linear_start_distance;   // mm
float linear_target_distance;  // mm
float linear_start_speed;      // rpm
float linear_target_speed;     // rpm
bool  linear_braking;

void linear_start(float distance /* mm */, bool stop) {
  linear_start_distance  = position_distance;                                              // mm
  linear_target_distance = position_distance + distance;                                   // mm
  linear_start_speed     = CLAMP_RPM((speed_measured_left + speed_measured_right) / 2.0);  // RPMs
  linear_target_speed    = stop ? 0.0 : SPEED_TO_RPM(SPEED_CRUISE);                        // RPMs
  linear_braking         = false;
}

bool linear_update() {
  // If we are there, then we are done.
  if (position_distance >= linear_target_distance) {
    speed_set(linear_target_speed, linear_target_speed);
    return true;
  }

  // Compute the braking distance.
  float current_speed = speed_setpoint_left;  // RPMs
  float braking_point = +INFINITY;            // mm
  if (current_speed > CLAMP_RPM(linear_target_speed)) {
    float brake_time = TICKS_TO_S((current_speed - CLAMP_RPM(linear_target_speed)) / ACCEL_TO_RPM(ACCEL_DEFAULT));  // s
    float brake_distance  = RPM_TO_SPEED(current_speed) * brake_time;       // mm
    brake_distance       -= 0.5 * ACCEL_DEFAULT * brake_time * brake_time;  // mm
    braking_point         = linear_target_distance - brake_distance;        // mm
  }

  // If we are in the braking zone, decelerate.
  if (position_distance >= braking_point) {
    float new_speed = speed_setpoint_left - ACCEL_TO_RPM(ACCEL_DEFAULT);  // RPMs
    speed_set(new_speed, new_speed);
    linear_braking = true;
  }
  // If we are not in the braking zone, and are under cruise speed, accelerate.
  else if (!linear_braking && current_speed < SPEED_TO_RPM(SPEED_CRUISE)) {
    float new_speed = speed_setpoint_left + ACCEL_TO_RPM(ACCEL_DEFAULT);  // RPMs
    speed_set(new_speed, new_speed);
  }

  speed_update();
  return false;
}
