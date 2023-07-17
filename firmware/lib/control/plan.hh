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

// state_t is the state of the current plan.
typedef enum : uint8_t {
  STATE_SCHEDULED,
  STATE_UNDERWAY,
  STATE_IMPLEMENTED,
} state_t;

typedef enum : uint8_t {
  // Plan is "implemented" as soon as the board is put into an idle state.
  TYPE_IDLE,
  // Plan is "implemented" once the led states are set.
  TYPE_LEDS,
  // Plan is "implemented" once the IR led states are set.
  TYPE_IR,
  // Plan is "implemented" as soon as the fixed power mode is set.
  TYPE_FIXED_POWER,
  // Plan is "implemented" as soon as the fixed speed mode is set.
  TYPE_FIXED_SPEED,
  // Plan is "implemented" once the linear motion has been completed.
  TYPE_LINEAR_MOTION,
  // Plan is "implemented" once the rotational motion has been completed.
  TYPE_ROTATIONAL_MOTION,
  // Plan is "implemented" once the calibration is complete.
  TYPE_SENSOR_CAL,
} type_t;

typedef struct {
  type_t  type;
  state_t state;
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
} plan_t;

// submit submits a new plan to be implemented.
void submit(plan_t plan);

// wait waits for the current plan to be implemented.
void wait();

// submit_and_wait submits a new plan, and wait for it to be implemented.
void submit_and_wait(plan_t plan);

}
