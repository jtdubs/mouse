#include <math.h>
#include <stddef.h>
#include <util/atomic.h>

#include "firmware/config.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/math.hh"
#include "firmware/platform/platform.hh"
#include "position_impl.hh"

namespace mouse::control::position {

namespace {
float distance_;  // in mms
float theta_;     // in radians
}  // namespace

void Init() {}

void Update() {
  int32_t left_delta, right_delta;
  platform::encoders::ReadDeltas(left_delta, right_delta);

  float left_distance  = static_cast<float>(left_delta) * config::kCountDistanceLeft;    // mm
  float right_distance = static_cast<float>(right_delta) * config::kCountDistanceRight;  // mm

  // Encoder updates in the same direction add to produce forward motion.
  float forward = (left_distance + right_distance) / 2.0f;  // mm

  // Encoder updates in opposite directions subtract to produce rotational motion.
  float rotation = (right_distance - left_distance) * config::kMMTheta;  // radians

  float distance, theta;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    distance = distance_;
    theta    = theta_;
  }

  distance += forward;
  theta    += rotation;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    distance_ = distance;
    theta_    = theta;
  }
}

void Read(float& distance, float& theta) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    distance = distance_;
    theta    = theta_;
  }
}

void Tare(float& distance, float& theta) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    distance  = distance_;
    theta     = theta_;
    distance_ = 0;
    theta_    = 0;
  }
}

}  // namespace mouse::control::position
