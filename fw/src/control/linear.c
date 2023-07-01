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
float linear_acceleration;     // rpm/tick
float linear_brake_distance;   // mm
float linear_coast_distance;   // mm

void linear_start(float distance /* mm */, bool coast) {
  linear_start_distance  = position_distance;
  linear_target_distance = position_distance + distance;
  linear_start_speed     = CLAMP_RPM((speed_measured_left + speed_measured_right) / 2.0);
  linear_target_speed    = coast ? SPEED_TO_RPM(SPEED_COAST) : 0.0;
  linear_acceleration    = ACCEL_TO_RPM(ACCEL_DEFAULT);

  if (linear_start_speed > linear_target_speed) {
    float brake_time       = TICKS_TO_S((linear_start_speed - linear_target_speed) / linear_acceleration);
    float brake_distance   = RPM_TO_SPEED(linear_start_speed) * brake_time;
    brake_distance        -= 0.5 * ACCEL_DEFAULT * brake_time * brake_time;
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
