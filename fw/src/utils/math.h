#pragma once

#include <math.h>
#include <stdint.h>

// clampf constrains x to the range [min, max].
inline float clampf(float x, float min, float max) {
  return fmaxf(min, fminf(max, x));
}

// abs16 returns the absolute value of x.
inline uint16_t abs16(int16_t x) {
  return x < 0 ? -x : x;
}

// abs8 returns the absolute value of x.
inline uint8_t abs8(int16_t x) {
  return x < 0 ? -x : x;
}

inline float clampthetaf(float theta) {
  theta = fmodf(theta, 2.0 * M_PI);
  if (theta < 0) {
    theta += 2.0 * M_PI;
  }
  return theta;
}
