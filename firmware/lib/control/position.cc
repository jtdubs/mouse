#include <math.h>
#include <stddef.h>
#include <util/atomic.h>

#include "config.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/math.hh"
#include "firmware/platform/platform.hh"
#include "position_impl.hh"

namespace position {

namespace {
float distance;  // in mms
float theta;     // in radians
}  // namespace

void Init() {}

void Update() {
  int32_t left_delta, right_delta;
  encoders::ReadDeltas(left_delta, right_delta);

  float left_distance  = static_cast<float>(left_delta) * kCountDistanceLeft;    // mm
  float right_distance = static_cast<float>(right_delta) * kCountDistanceRight;  // mm

  // Encoder updates in the same direction add to produce forward motion.
  float forward = (left_distance + right_distance) / 2.0f;  // mm

  // Encoder updates in opposite directions subtract to produce rotational motion.
  float rotation = (right_distance - left_distance) * kMMTheta;  // radians

  float distance, theta;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    distance = position::distance;
    theta    = position::theta;
  }

  distance += forward;
  theta    += rotation;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    position::distance = distance;
    position::theta    = theta;
  }
}

void Read(float& distance, float& theta) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    distance = position::distance;
    theta    = position::theta;
  }
}

void Tare(float& distance, float& theta) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    distance           = position::distance;
    theta              = position::theta;
    position::distance = 0;
    position::theta    = 0;
  }
}

}  // namespace position
