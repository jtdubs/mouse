//
// System: control
// Module: plan
//
// Purpose:
// - Define the plan data structures and related methods.
//
// Design:
// - There is a single active plan.
// - The plan can be set with Submit().
// - The plan can be waited on with Wait(), which is a busy-loop.
// - The plan state is updated by the control loop.
//
#pragma once

#include <stdbool.h>
#include <stdint.h>

#if not defined(__AVR__)
#include <ostream>
#endif

namespace mouse::control::plan {

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

#pragma pack(push, 1)
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
#pragma pack(pop)

// submit submits a new plan to be implemented.
void Submit(Plan plan);

// wait waits for the current plan to be implemented.
void Wait();

// SubmitAndWait submits a new plan, and wait for it to be implemented.
void SubmitAndWait(Plan plan);

#if not defined(__AVR__)
[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const State state) {
  switch (state) {
    case State::Scheduled:
      o << "Scheduled";
      break;
    case State::Underway:
      o << "Underway";
      break;
    case State::Implemented:
      o << "Implemented";
      break;
    default:
      o << "Unknown";
      break;
  }
  return o;
}

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const Type t) {
  switch (t) {
    case Type::Idle:
      o << "Idle";
      break;
    case Type::LEDs:
      o << "LEDs";
      break;
    case Type::IR:
      o << "IR";
      break;
    case Type::FixedPower:
      o << "FixedPower";
      break;
    case Type::FixedSpeed:
      o << "FixedSpeed";
      break;
    case Type::LinearMotion:
      o << "LinearMotion";
      break;
    case Type::RotationalMotion:
      o << "RotationalMotion";
      break;
    case Type::SensorCal:
      o << "SensorCal";
      break;
    default:
      o << "Unknown";
      break;
  }
  return o;
}

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const Plan plan) {
  o << plan.state << " ";
  switch (plan.type) {
    case Type::Idle:
      o << "idle: {}";
      break;
    case Type::LEDs:
      o << "leds: { ";
      o << plan.data.leds.left << ", ";
      o << plan.data.leds.right << ", ";
      o << plan.data.leds.onboard << "}";
      break;
    case Type::IR:
      o << "ir: " << plan.data.ir.on;
      break;
    case Type::FixedPower:
      o << "power: {" << plan.data.power.left << ", " << plan.data.power.right << "}";
      break;
    case Type::FixedSpeed:
      o << "speed: {" << plan.data.speed.left << ", " << plan.data.speed.right << "}";
      break;
    case Type::LinearMotion:
      o << "linear: {" << plan.data.linear.position << ", " << plan.data.linear.stop << "}";
      break;
    case Type::RotationalMotion:
      o << "rotational: {" << plan.data.rotational.d_theta << " }";
      break;
    case Type::SensorCal:
      o << "sensor_cal: {}";
      break;
    default:
      break;
  }
  // o << "}";
  return o;
}
#endif

}  // namespace mouse::control::plan
