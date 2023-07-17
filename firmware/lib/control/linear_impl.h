#pragma once

#include <stdbool.h>

#include "linear.h"

#pragma pack(push, 1)
typedef struct {
  float target_position;  // mm
  float target_speed;     // mm/s
  float wall_error;
  bool  leds_prev_state;
} linear_state_t;
#pragma pack(pop)

// linear_init initializes the linear motion module.
void linear_init();

// linear_start starts a linear motion plan.
void linear_start(float position /* mm */, bool stop);

// linear_tick updates the platform to implement the plan.
// Returns true if the plan is complete.
bool linear_tick();

// linear_state reads the current linear state.
void linear_state(linear_state_t *state);
