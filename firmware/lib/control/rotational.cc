#include <stdbool.h>
#include <util/atomic.h>

#include "config.h"
#include "firmware/lib/utils/math.h"
#include "position.h"
#include "rotational_impl.h"
#include "speed.h"

static rotational_state_t state;

void rotational_init() {}

void rotational_start(float dtheta /* radians */) {
  rotational_state_t s;
  float              pdistance;
  position_read(&pdistance, &s.start_theta);

  s.target_theta = s.start_theta + dtheta;
  s.direction    = dtheta > 0;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state = s;
  }
}

bool rotational_tick() {
  float position_distance, position_theta;
  position_read(&position_distance, &position_theta);

  rotational_state_t s;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state;
  }

  // If we are there, then we are done.
  if (fabsf(position_theta - s.target_theta) < (MM_THETA * 2.0f)) {
    speed_set(0, 0);
    return true;
  }

  // Otherwise, calculate fixed speeds including the wheel bias.
  float rpm         = MIN_MOTOR_RPM * 1.5;
  float left_speed  = rpm;
  float right_speed = rpm * ((1.0 + WHEEL_BIAS) / (1.0 - WHEEL_BIAS));

  if (s.direction) {
    speed_set(-left_speed, right_speed);
  } else {
    speed_set(left_speed, -right_speed);
  }

  return false;
}

void rotational_state(rotational_state_t *s) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *s = state;
  }
}
