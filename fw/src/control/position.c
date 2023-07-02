#include "position.h"

#include <math.h>

#include "control/config.h"
#include "platform/encoders.h"
#include "utils/math.h"

float position_distance;  // in mms
float position_theta;     // in radians

void position_init() {}

void position_read() {
  float left_distance  = ((float)encoders_left_delta) * COUNT_DISTANCE_LEFT;    // mm
  float right_distance = ((float)encoders_right_delta) * COUNT_DISTANCE_RIGHT;  // mm

  float forward  = (left_distance + right_distance) / 2.0;       // mm
  float rotation = (right_distance - left_distance) * MM_THETA;  // radians

  position_distance += forward;
  position_theta    += rotation;
}

void position_clear() {
  position_distance = 0;
  position_theta    = 0;
}
