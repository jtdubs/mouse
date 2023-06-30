#include "position.h"

#include <math.h>

#include "platform/encoders.h"

#define WHEEL_BASE 90.0
#define WHEEL_DIAMETER 32.0
#define ENCODER_TICKS_PER_ROTATION 240

static const float WheelCircumference = M_PI * WHEEL_DIAMETER;
static const float TickDistance       = WheelCircumference / ENCODER_TICKS_PER_ROTATION;
static const float TickDTheta         = TickDistance / WHEEL_BASE;

float position_distance;  // in mms
float position_theta;     // in radians

void position_init() {}

void position_read() {
  float forward  = (encoders_left_delta + encoders_right_delta) / 2.0;
  float rotation = (encoders_right_delta - encoders_left_delta);

  position_distance = fma(forward, TickDistance, position_distance);
  position_theta    = fma(rotation, TickDTheta, position_theta);
  position_theta    = fmodf(position_theta, 2 * M_PI);
  if (position_theta < 0) {
    position_theta += 2 * M_PI;
  }
}
