#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum : uint8_t {
  PLAN_STATE_SCHEDULED,
  PLAN_STATE_UNDERWAY,
  PLAN_STATE_IMPLEMENTED,
} plan_state_t;

typedef enum : uint8_t {
  // Plan is "implemented" as soon as the board is put into an idle state.
  PLAN_TYPE_IDLE,
  // Plan is "implemented" as soon as the fixed power mode is set.
  PLAN_TYPE_FIXED_POWER,
  // Plan is "implemented" as soon as the fixed speed mode is set.
  PLAN_TYPE_FIXED_SPEED,
  // Plan is "implemented" once the linear motion has been completed.
  PLAN_TYPE_LINEAR_MOTION,
  // Plan is "implemented" once the rotational motion has been completed.
  PLAN_TYPE_ROTATIONAL_MOTION,
} plan_type_t;

typedef struct {
  plan_type_t  type;
  plan_state_t state;
  union {
    // PLAN_FIXED_POWER
    struct {
      int16_t left;   // dimensionless, range [-511, 511]
      int16_t right;  // dimensionless, range [-511, 511]
    } power;
    // PLAN_FIXED_SPEED
    struct {
      float left;   // rpm
      float right;  // rpm
    } speed;
    // PLAN_LINEAR_MOTION
    struct {
      float distance;  // mm
      bool  coast;     // end at coasting speed, or stopped?
    } linear;
    // PLAN_ROTATIONAL_MOTION
    struct {
      float d_theta;  // radians
    } rotational;
  } data;
} plan_t;

// current_plan is the current plan
extern volatile plan_t current_plan;

// plan_init initializes the plan module.
void plan_init();

// plan_submit submits a new plan to be implemented.
void plan_submit(plan_t *plan);

// plan_wait waits for the current plan to be implemented.
void plan_wait();

// plan_submit_and_wait submits a new plan, and wait for it to be implemented.
void plan_submit_and_wait(plan_t *plan);

// plan_set_state sets the current plan state.
void plan_set_state(plan_state_t state);
