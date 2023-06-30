#pragma once

#include <stdbool.h>
#include <stdint.h>

#define PLAN_IDLE 0
#define PLAN_FIXED_POWER 1
#define PLAN_FIXED_SPEED 2

typedef struct {
  uint8_t type;
  bool    implemented;
  union {
    // PLAN_FIXED_POWER
    struct {
      int16_t left;
      int16_t right;
    } power;
    // PLAN_FIXED_SPEED
    struct {
      float left;
      float right;
    } speed;
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
