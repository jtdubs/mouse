#include "position.h"

#include <math.h>

#include "control/config.h"
#include "platform/encoders.h"
#include "utils/math.h"

float position_distance;  // in mms
float position_theta;     // in radians

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

  position_distance += forward;
  position_theta    += rotation;
}

void position_clear() {
  position_distance = 0;
  position_theta    = 0;
}
