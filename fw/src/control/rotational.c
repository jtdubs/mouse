#include "rotational.h"

#include <stdbool.h>
#include <util/atomic.h>

#include "control/config.h"
#include "control/position.h"
#include "control/speed.h"
#include "utils/math.h"

float rotational_start_theta;   // radians
float rotational_target_theta;  // radians
float rotational_target_speed;  // radians/s
bool  rotational_direction;     // true = positive, false = negative

void rotational_init() {}

void rotational_start(float dtheta /* radians */) {
  rotational_start_theta  = position_theta;             // radians
  rotational_target_theta = position_theta + dtheta;    // radians
  rotational_target_speed = SPEED_CRUISE / WHEEL_BASE;  // radians/s
  rotational_direction    = dtheta > 0;
}

bool rotational_update() {
  // If we are there, then we are done.
  if (fabsf(position_theta - rotational_target_theta) < (MM_THETA * 5.0f)) {
    speed_set(0, 0);
    return true;
  }

  // Otherwies, calculate fixed speeds including the wheel bias.
  float rpm         = MIN_MOTOR_RPM * 2.0;
  float left_speed  = rpm;
  float right_speed = rpm * ((1.0 + WHEEL_BIAS) / (1.0 - WHEEL_BIAS));

  if (rotational_direction) {
    speed_set(-left_speed, right_speed);
  } else {
    speed_set(left_speed, -right_speed);
  }

  speed_update();
  return false;
}
