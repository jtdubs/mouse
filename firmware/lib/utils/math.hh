#pragma once

#include <math.h>
#include <stdint.h>

namespace mouse::math {

// min8 returns minimum of x and y.
inline uint8_t min8(uint8_t x, uint8_t y) {
  return (x <= y) ? x : y;
}

// abs8 returns the absolute value of x.
inline uint8_t abs8(int16_t x) {
  return x < 0 ? -x : x;
}

// math::abs16 returns the absolute value of x.
inline uint16_t abs16(int16_t x) {
  return x < 0 ? -x : x;
}

// math::clampf constrains x to the range [min, max].
inline float clampf(float x, float min, float max) {
  return fmaxf(min, fminf(max, x));
}

// clampthetaf constrains theta to the range [0, 2pi).
inline float clampthetaf(float theta) {
  theta = fmodf(theta, 2.0 * M_PI);
  if (theta < 0) {
    theta += 2.0 * M_PI;
  }
  return theta;
}

}  // namespace mouse::math
