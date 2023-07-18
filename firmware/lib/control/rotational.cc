#include <stdbool.h>
#include <util/atomic.h>

#include "config.hh"
#include "firmware/lib/utils/math.hh"
#include "position.hh"
#include "rotational_impl.hh"
#include "speed.hh"

namespace rotational {

namespace {
State state;
}

void init() {}

void start(float dtheta /* radians */) {
  State s;
  float distance;
  position::read(distance, s.start_theta);

  s.target_theta = s.start_theta + dtheta;
  s.direction    = dtheta > 0;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state = s;
  }
}

bool tick() {
  float distance, theta;
  position::read(distance, theta);

  State s;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state;
  }

  // If we are there, then we are done.
  if (fabsf(theta - s.target_theta) < (MM_THETA * 2.0f)) {
    speed::set(0, 0);
    return true;
  }

  // Otherwise, calculate fixed speeds including the wheel bias.
  float rpm         = MIN_MOTOR_RPM * 1.5;
  float left_speed  = rpm;
  float right_speed = rpm * ((1.0 + WHEEL_BIAS) / (1.0 - WHEEL_BIAS));

  if (s.direction) {
    speed::set(-left_speed, right_speed);
  } else {
    speed::set(left_speed, -right_speed);
  }

  return false;
}

void read(State &s) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state;
  }
}

}  // namespace rotational
