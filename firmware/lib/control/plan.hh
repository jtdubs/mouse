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
void Submit(Plan plan);

// wait waits for the current plan to be implemented.
void Wait();

// SubmitAndWait submits a new plan, and wait for it to be implemented.
void SubmitAndWait(Plan plan);

#if not defined(__AVR__)
std::ostream &operator<<(std::ostream &o, const State state) {
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

std::ostream &operator<<(std::ostream &o, const Type t) {
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

std::ostream &operator<<(std::ostream &o, const Plan *plan) {
  o << "plan::Plan{" << std::endl;
  o << "  type: " << plan->type << std::endl;
  o << "  state: " << plan->state << std::endl;
  o << "  data: {" << std::endl;
  switch (plan->type) {
    case Type::Idle:
      o << "    idle: {}" << std::endl;
      break;
    case Type::LEDs:
      o << "    leds: {" << std::endl;
      o << "      left: " << plan->data.leds.left << std::endl;
      o << "      right: " << plan->data.leds.right << std::endl;
      o << "      onboard: " << plan->data.leds.onboard << std::endl;
      o << "    }" << std::endl;
      break;
    case Type::IR:
      o << "    ir: {" << std::endl;
      o << "      on: " << plan->data.ir.on << std::endl;
      o << "    }" << std::endl;
      break;
    case Type::FixedPower:
      o << "    power: {" << std::endl;
      o << "      left: " << plan->data.power.left << std::endl;
      o << "      right: " << plan->data.power.right << std::endl;
      o << "    }" << std::endl;
      break;
    case Type::FixedSpeed:
      o << "    speed: {" << std::endl;
      o << "      left: " << plan->data.speed.left << std::endl;
      o << "      right: " << plan->data.speed.right << std::endl;
      o << "    }" << std::endl;
      break;
    case Type::LinearMotion:
      o << "    linear: {" << std::endl;
      o << "      position: " << plan->data.linear.position << std::endl;
      o << "      stop: " << plan->data.linear.stop << std::endl;
      o << "    }" << std::endl;
      break;
    case Type::RotationalMotion:
      o << "    rotational: {" << std::endl;
      o << "      d_theta: " << plan->data.rotational.d_theta << std::endl;
      o << "    }" << std::endl;
      break;
    case Type::SensorCal:
      o << "    sensor_cal: {}" << std::endl;
      break;
    default:
      break;
  }
  o << "  }" << std::endl;
  o << "}";
  return o;
}
#endif

}  // namespace plan
