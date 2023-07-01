#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t plan_state_t;

#define PLAN_STATE_SCHEDULED 0
#define PLAN_STATE_UNDERWAY 1
#define PLAN_STATE_IMPLEMENTED 2

// Plan is "implemented" as soon as the board is put into an idle state.
#define PLAN_IDLE 0

// Plan is "implemented" as soon as the fixed power mode is set.
#define PLAN_FIXED_POWER 1

// Plan is "implemented" as soon as the fixed speed mode is set.
#define PLAN_FIXED_SPEED 2

// Plan is "implemented" once the linear motion has been completed.
#define PLAN_LINEAR_MOTION 3

// Plan is "implemented" once the rotational motion has been completed.
#define PLAN_ROTATIONAL_MOTION 4

typedef struct {
  uint8_t      type;
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
      float distance;    // mm
      float exit_speed;  // mm/s
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
