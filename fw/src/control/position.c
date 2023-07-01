#include "position.h"

#include <math.h>

#include "control/config.h"
#include "platform/encoders.h"
#include "utils/math.h"

float position_distance;  // in mms
float position_theta;     // in radians

void position_init() {}

void position_read() {
  float forward  = (encoders_left_delta + encoders_right_delta) / 2.0;
  float rotation = (encoders_right_delta - encoders_left_delta);

  position_distance = fma(forward, COUNT_DISTANCE, position_distance);
  position_theta    = clampthetaf(fma(rotation, COUNT_THETA, position_theta));
}

void position_clear() {
  position_distance = 0;
  position_theta    = 0;
}
