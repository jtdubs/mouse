//
// System: control
// Module: plan
//
// Purpose:
// - Define the plan data structures and related methods.
//
// Design:
// - There is a single active plan.
// - The plan can be set with submit().
// - The plan can be waited on with wait(), which is a busy-loop.
// - The plan state is updated by the control loop.
//
#pragma once

#include <stdbool.h>
#include <stdint.h>

namespace plan {

// State is the state of the current plan.
enum class State : uint8_t {
  Scheduled,
  Underway,
  Implemented,
};

enum class Type : uint8_t {
  // Plan is "implemented" as soon as the board is put into an idle state.
  Idle,
  // Plan is "implemented" once the led states are set.
  LEDs,
  // Plan is "implemented" once the IR led states are set.
  IR,
  // Plan is "implemented" as soon as the fixed power mode is set.
  FixedPower,
  // Plan is "implemented" as soon as the fixed speed mode is set.
  FixedSpeed,
  // Plan is "implemented" once the linear motion has been completed.
  LinearMotion,
  // Plan is "implemented" once the rotational motion has been completed.
  RotationalMotion,
  // Plan is "implemented" once the calibration is complete.
  SensorCal,
};

struct Plan {
  Type  type;
  State state;
  union {
    // TYPE_IDLE
    struct {
    } idle;
    // TYPE_LEDS
    struct {
      bool left;
      bool right;
      bool onboard;
    } leds;
    // TYPE_IR
    struct {
      bool on;
    } ir;
    // TYPE_FIXED_POWER
    struct {
      int16_t left;   // dimensionless, range [-511, 511]
      int16_t right;  // dimensionless, range [-511, 511]
    } power;
    // TYPE_FIXED_SPEED
    struct {
      float left;   // rpm
      float right;  // rpm
    } speed;
    // TYPE_LINEAR_MOTION
    struct {
      float position;  // mm
      bool  stop;
    } linear;
    // TYPE_ROTATIONAL_MOTION
    struct {
      float d_theta;  // radians
    } rotational;
    // TYPE_SENSOR_CAL
    struct {
    } sensor_cal;
  } data;
};

// submit submits a new plan to be implemented.
void submit(Plan plan);

// wait waits for the current plan to be implemented.
void wait();

// submit_and_wait submits a new plan, and wait for it to be implemented.
void submit_and_wait(Plan plan);

}  // namespace plan
