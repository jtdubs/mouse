#include "position.h"

#include <math.h>
#include <stddef.h>
#include <util/atomic.h>

#include "control/config.h"
#include "platform/encoders.h"
#include "utils/assert.h"
#include "utils/math.h"

static float position_distance;  // in mms
static float position_theta;     // in radians

void position_init() {}

void position_update() {
  int32_t left_delta, right_delta;
  encoders_read_deltas(&left_delta, &right_delta);

  float left_distance  = ((float)left_delta) * COUNT_DISTANCE_LEFT;    // mm
  float right_distance = ((float)right_delta) * COUNT_DISTANCE_RIGHT;  // mm

  // Encoder updates in the same direction add to produce forward motion.
  float forward = (left_distance + right_distance) / 2.0f;  // mm

  // Encoder updates in opposite directions subtract to produce rotational motion.
  float rotation = (right_distance - left_distance) * MM_THETA;  // radians

  float distance, theta;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    distance = position_distance;
    theta    = position_theta;
  }

  distance += forward;
  theta    += rotation;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    position_distance = distance;
    position_theta    = theta;
  }
}

void position_clear() {
  position_distance = 0;
  position_theta    = 0;
}

void position_read(float* distance, float* theta) {
  assert(ASSERT_POSITION + 0, distance != NULL);
  assert(ASSERT_POSITION + 1, theta != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *distance = position_distance;
    *theta    = position_theta;
  }
}
