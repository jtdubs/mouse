//
// System: control
// Module: config
//
// Purpose:
// - Provide tunable configuration values, and related conversion functions.
//
#pragma once

#include <math.h>
#include <stdint.h>

#include "firmware/lib/utils/math.hh"

// Robot dimensions
constexpr float kBoardWidth    = 81.25;  // mm
constexpr float kBoardLength   = 99.00;  // mm
constexpr float kWheelBase     = 72.48;  // mm
constexpr float kWheelDiameter = 32.50;  // mm
constexpr float kAxleOffset    = 36.00;  // mm, from back of board

// Maze dimensions
constexpr uint8_t kMazeWidth  = 16;                 // dimensionless
constexpr uint8_t kMazeHeight = 16;                 // dimensionless
constexpr float   kCellSize   = 180.0;              // mm
constexpr float   kCellSize2  = (kCellSize / 2.0);  // mm

// Sensor performance
constexpr uint16_t kSensorEmergencyStop   = 800;
constexpr uint16_t kSensorSideDefaultCal  = 200;
constexpr uint16_t kSensorFrontDefaultCal = 110;

// Motor parameters
constexpr float kCountsPerRevolution = 240;  // dimensionless

// Motor performance (rpm = m * power + b)
// Ref: https://www.desmos.com/calculator/dxhrixaxre
constexpr float kMinMotorRPM = 35;     // RPM
constexpr float kLeftMotorM  = 2.760;  // RPM/power
constexpr float kLeftMotorB  = -67.2;  // RPM
constexpr float kRightMotorM = 2.695;  // RPM/power
constexpr float kRightMotorB = -57.7;  // RPM

// Software parameters
constexpr float kControlPeriod = 0.005;  // s

// Delicious fudge
constexpr float kWheelBias = 0.000;  // dimensionless

// Derived constants
constexpr float kWheelCircumference = M_PI * kWheelDiameter;                       // mm
constexpr float kCountDistance      = kWheelCircumference / kCountsPerRevolution;  // mm
constexpr float kCountDistanceLeft  = (1.0 + kWheelBias) * kCountDistance;         // mm
constexpr float kCountDistanceRight = (1.0 - kWheelBias) * kCountDistance;         // mm
constexpr float kCountTheta         = kCountDistance / kWheelBase;                 // radians
constexpr float kMMTheta            = 1.0 / kWheelBase;                            // radians

// Performance characteristics
constexpr float kMaxMotorPower = 200;              // dimensionless
constexpr float kMaxSpeed      = 1000;             // mm/s
constexpr float kAccel1G       = 9810;             // mm/s^2
constexpr float kAccelDefault  = 0.15 * kAccel1G;  // mm/s^2
constexpr float kSpeedCruise   = 500;              // mm/s

// Pre-tuned PIDs
// #define ALLOW_SPEED_PID_TUNING
// #define ALLOW_WALL_PID_TUNING
// #define ALLOW_ANGLE_PID_TUNING
constexpr float kSpeedKp    = 0.10;                   // dimensionless
constexpr float kSpeedKi    = 0.02 * kControlPeriod;  // dimensionless
constexpr float kSpeedKd    = 0.00 / kControlPeriod;  // dimensionless
constexpr float kSpeedAlpha = 0.50;                   // dimensionless
constexpr float kWallKp     = 0.40;                   // dimensionless
constexpr float kWallKi     = 0.20 * kControlPeriod;  // dimensionless
constexpr float kWallKd     = 0.00 / kControlPeriod;  // dimensionless
constexpr float kWallAlpha  = 0.50;                   // dimensionless
constexpr float kAngleKp    = 0.00;                   // dimensionless
constexpr float kAngleKi    = 0.00 * kControlPeriod;  // dimensionless
constexpr float kAngleKd    = 0.00 / kControlPeriod;  // dimensionless
constexpr float kAngleAlpha = 0.50;                   // dimensionless

// Conversion functions
inline static float SpeedToRPM(float speed /* mm/s */) {
  return speed * (60.0 / (kWheelDiameter * M_PI));
}

inline static float RPMToSpeed(float rpm) {
  return rpm * (kWheelDiameter * M_PI / 60.0);
}

inline static float AccelToRPM(float accel /* mm/s^2 */) {
  return accel * SpeedToRPM(kControlPeriod);
}

inline static float SecondsToTicks(float s) {
  return s * (1.0 / kControlPeriod);
}
inline static float TicksToSeconds(float ticks) {
  return ticks * kControlPeriod;
}

inline static float ClampRPM(float rpm) {
  return math::clampf(rpm, kMinMotorRPM, SpeedToRPM(kMaxSpeed));
}

inline static float CountsToRPM(float counts /* per control period */) {
  return counts * (60.0 / (kCountsPerRevolution * kControlPeriod));
}

inline static float LeftPowerToRPM(float power) {
  return (power * kLeftMotorM) + kLeftMotorB;
}

inline static float LeftRPMToPower(float rpm) {
  return (rpm - kLeftMotorB) * (1.0 / kLeftMotorM);
}

inline static float RightPowerToRPM(float power) {
  return (power * kRightMotorM) + kRightMotorB;
}

inline static float RightRPMToPower(float rpm) {
  return (rpm - kRightMotorB) * (1.0 / kRightMotorM);
}
