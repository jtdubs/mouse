#include "linear.h"

#include <stdbool.h>

#include "control/position.h"
#include "control/speed.h"
#include "utils/math.h"

float linear_start_distance;   // mm
float linear_target_distance;  // mm
float linear_start_speed;      // rpm
float linear_target_speed;     // rpm
float linear_acceleration;     // rpm/tick
float linear_brake_distance;   // mm

void linear_start(float distance /* mm */, float speed /* mm/s */, float acceleration /* mm/s^2 */) {
  linear_start_distance  = position_distance;
  linear_target_distance = position_distance + distance;
  linear_start_speed     = clampf((speed_measured_left + speed_measured_right) / 2.0, 30.0, 1000.0);
  linear_target_speed    = speed * (60.0 / (32.0 * M_PI));
  linear_acceleration    = acceleration * ((60.0 * 0.005) / (32.0 * M_PI));

  if (linear_start_speed > linear_target_speed) {
    float brake_ticks      = (linear_start_speed - linear_target_speed) / linear_acceleration;
    float brake_distance   = ((linear_start_speed * (32.0 * M_PI / 60.0) * 0.005) * brake_ticks);
    brake_distance        -= (0.5 * (acceleration * (0.005 * .005)) * brake_ticks * brake_ticks);
    brake_distance        -= 3.0;  // fudge factor :p
    linear_brake_distance  = linear_target_distance - brake_distance;
  } else {
    linear_brake_distance = linear_target_distance;
  }
}

bool linear_update() {
  // if we are there, then we are done.
  if (position_distance >= linear_target_distance) {
    speed_set(linear_target_speed, linear_target_speed);
    return true;
  }

  // if we are under target speed, accelerate
  if (speed_setpoint_left < linear_target_speed) {
    float new_speed = speed_setpoint_left + linear_acceleration;
    speed_set(new_speed, new_speed);
  }

  // if we are in the braking zone, decelerate
  if (position_distance >= linear_brake_distance) {
    float new_speed = speed_setpoint_left - linear_acceleration;
    speed_set(new_speed, new_speed);
  }

  speed_update();
  return false;
}
