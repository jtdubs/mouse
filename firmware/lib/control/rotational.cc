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

void Init() {}

void Start(float dtheta /* radians */) {
  float distance, theta;
  position::Read(distance, theta);

  State s = {
      .start_theta  = theta,
      .target_theta = theta + dtheta,
      .direction    = dtheta > 0,
  };

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state = s;
  }
}

bool Tick() {
  float distance, theta;
  position::Read(distance, theta);

  State s;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state;
  }

  // If we are there, then we are done.
  if (fabsf(theta - s.target_theta) < kMMTheta) {
    speed::Set(0, 0);
    return true;
  }

  // Otherwise, calculate fixed speeds including the wheel bias.
  float rpm         = kMinMotorRPM * 1.5;
  float left_speed  = rpm;
  float right_speed = rpm * ((1.0 + kWheelBias) / (1.0 - kWheelBias));

  if (s.direction) {
    speed::Set(-left_speed, right_speed);
  } else {
    speed::Set(left_speed, -right_speed);
  }

  return false;
}

void Read(State &s) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state;
  }
}

}  // namespace rotational
