#include <math.h>
#include <stddef.h>
#include <util/atomic.h>

#include "config.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/math.hh"
#include "firmware/platform/encoders.hh"
#include "position_impl.hh"

namespace position {

namespace {
float distance;  // in mms
float theta;     // in radians
}  // namespace

void init() {}

void update() {
  int32_t left_delta, right_delta;
  encoders::read_deltas(&left_delta, &right_delta);

  float left_distance  = ((float)left_delta) * COUNT_DISTANCE_LEFT;    // mm
  float right_distance = ((float)right_delta) * COUNT_DISTANCE_RIGHT;  // mm

  // Encoder updates in the same direction add to produce forward motion.
  float forward = (left_distance + right_distance) / 2.0f;  // mm

  // Encoder updates in opposite directions subtract to produce rotational motion.
  float rotation = (right_distance - left_distance) * MM_THETA;  // radians

  float d, t;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    d = distance;
    t = theta;
  }

  d += forward;
  t += rotation;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    distance = d;
    theta    = t;
  }
}

void read(float* d, float* t) {
  assert(assert::POSITION + 0, d != NULL);
  assert(assert::POSITION + 1, t != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *d = distance;
    *t = theta;
  }
}

void tare(float* d, float* t) {
  assert(assert::POSITION + 2, d != NULL);
  assert(assert::POSITION + 3, t != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *d       = distance;
    *t       = theta;
    distance = 0;
    theta    = 0;
  }
}

}  // namespace position
