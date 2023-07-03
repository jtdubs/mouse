#pragma once

#include <math.h>
#include <stdint.h>

#include "utils/math.h"

// Robot dimensions
constexpr float BOARD_WIDTH    = 81.25;  // mm
constexpr float BOARD_LENGTH   = 99.00;  // mm
constexpr float WHEEL_BASE     = 90.50;  // mm
constexpr float WHEEL_DIAMETER = 32.50;  // mm

// Motor parameters
constexpr float COUNTS_PER_REVOLUTION = 240;  // dimensionless

// Motor performance (rpm = m * power + b)
// Ref: https://www.desmos.com/calculator/dxhrixaxre
constexpr float MIN_MOTOR_RPM = 35;     // RPM
constexpr float LEFT_MOTOR_M  = 2.760;  // RPM/power
constexpr float LEFT_MOTOR_B  = -67.2;  // RPM
constexpr float RIGHT_MOTOR_M = 2.695;  // RPM/power
constexpr float RIGHT_MOTOR_B = -57.7;  // RPM

// Software parameters
constexpr float CONTROL_PERIOD = 0.005;  // s

// Delicious fudge
constexpr float WHEEL_BIAS = 0.002;  // dimensionless

// Derived constants
constexpr float WHEEL_CIRCUMFERENCE  = M_PI * WHEEL_DIAMETER;                        // mm
constexpr float COUNT_DISTANCE       = WHEEL_CIRCUMFERENCE / COUNTS_PER_REVOLUTION;  // mm
constexpr float COUNT_DISTANCE_LEFT  = (1.0 + WHEEL_BIAS) * COUNT_DISTANCE;          // mm
constexpr float COUNT_DISTANCE_RIGHT = (1.0 - WHEEL_BIAS) * COUNT_DISTANCE;          // mm
constexpr float COUNT_THETA          = COUNT_DISTANCE / WHEEL_BASE;                  // radians
constexpr float MM_THETA             = 1.0 / WHEEL_BASE;                             // radians

// Performance characteristics
constexpr float MAX_MOTOR_POWER      = 200;                    // dimensionless
constexpr float MAX_SPEED            = 1000;                   // mm/s
constexpr float SPEED_KP             = 0.1;                    // dimensionless
constexpr float SPEED_KI             = 0.02 * CONTROL_PERIOD;  // dimensionless
constexpr float SPEED_LOW_PASS_ALPHA = 0.5;                    // dimensionless
constexpr float ACCEL_1G             = 9810;                   // mm/s^2
constexpr float ACCEL_DEFAULT        = 0.15 * ACCEL_1G;        // mm/s^2
constexpr float SPEED_CRUISE         = 500;                    // mm/s

// Conversion functions
inline static float SPEED_TO_RPM(float speed /* mm/s */) {
  return speed * (60.0 / (WHEEL_DIAMETER * M_PI));
}

inline static float RPM_TO_SPEED(float rpm) {
  return rpm * (WHEEL_DIAMETER * M_PI / 60.0);
}

inline static float ACCEL_TO_RPM(float accel /* mm/s^2 */) {
  return accel * SPEED_TO_RPM(CONTROL_PERIOD);
}

inline static float S_TO_TICKS(float s) {
  return s * (1.0 / CONTROL_PERIOD);
}
inline static float TICKS_TO_S(float ticks) {
  return ticks * CONTROL_PERIOD;
}

inline static float CLAMP_RPM(float rpm) {
  return clampf(rpm, MIN_MOTOR_RPM, SPEED_TO_RPM(MAX_SPEED));
}

inline static float COUNTS_TO_RPM(float counts /* per control period */) {
  return counts * (60.0 / (COUNTS_PER_REVOLUTION * CONTROL_PERIOD));
}

inline static float LEFT_POWER_TO_RPM(float power) {
  return (power * LEFT_MOTOR_M) + LEFT_MOTOR_B;
}

inline static float LEFT_RPM_TO_POWER(float rpm) {
  return (rpm - LEFT_MOTOR_B) * (1.0 / LEFT_MOTOR_M);
}

inline static float RIGHT_POWER_TO_RPM(float power) {
  return (power * RIGHT_MOTOR_M) + RIGHT_MOTOR_B;
}

inline static float RIGHT_RPM_TO_POWER(float rpm) {
  return (rpm - RIGHT_MOTOR_B) * (1.0 / RIGHT_MOTOR_M);
}
