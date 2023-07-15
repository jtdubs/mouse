//
// System: control
// Module: linear
//
// Purpose:
// - Handles linear motion plans.
//
// Design:
// - Drives a specified distance, and optionally stops at the end of the motion.
// - The mouse's velocity-over-time is a trapezoid with three phases:
//   - A constant acceleration phase to a cruising speed.
//   - A constant velocity phase.
//   - A constant decelleration phase to a stop (if requested).
//
#pragma once

#include <stdbool.h>

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

// PID tuning functions.
void linear_wall_tune(float kp, float ki, float kd, float alpha);
void linear_angle_tune(float kp, float ki, float kd, float alpha);

// linear_state reads the current linear state.
void linear_state(linear_state_t *state);
