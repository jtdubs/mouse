#include "linear.h"

#include <stdbool.h>

#include "control/position.h"
#include "control/speed.h"

float linear_start_distance;   // mm
float linear_target_distance;  // mm
float linear_start_speed;      // rpm
float linear_target_speed;     // rpm
float linear_acceleration;     // rpm/tick
float linear_brake_distance;   // mm

void linear_start(float distance /* mm */, float speed /* mm/s */, float acceleration /* mm/s^2 */) {
  linear_start_distance  = position_distance;
  linear_target_distance = position_distance + distance;
  linear_start_speed     = (speed_measured_left + speed_measured_right) / 2.0;
  linear_target_speed    = speed * (60.0 / (32.0 * M_PI));
  linear_acceleration    = acceleration * (60.0 * 0.000005 / 32.0);

  if (linear_start_speed > linear_target_speed) {
    float brake_ticks     = (linear_target_speed - linear_start_speed) / linear_acceleration;
    linear_brake_distance = linear_start_distance + (linear_start_speed * (60 * 32 / 200) * brake_ticks)
                          - (0.5 * linear_acceleration * (60 * 32 / 200) * brake_ticks * brake_ticks);
  } else {
    linear_brake_distance = linear_target_distance;
  }
}

bool linear_update() {
  // if we are there, then we are done.
  if (position_distance >= linear_target_distance) {
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
