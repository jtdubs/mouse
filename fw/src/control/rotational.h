//
// System: control
// Module: rotational
//
// Purpose:
// - Handles rotational motion plans.
//
// Design:
// - Rotate a specific number of radians and stop.
// - Rotation currently happens a a slow-ish fixed speed.
//
#pragma once

#include <stdbool.h>

#pragma pack(push, 1)
typedef struct {
  float start_theta;   // radians
  float target_theta;  // radians
  bool  direction;     // true = positive, false = negative
} rotational_state_t;
#pragma pack(pop)

// rotational_init initializes the rotational motion module.
void rotational_init();

// rotational_start starts a rotational motion plan.
void rotational_start(float dtheta /* radians */);

// rotational_tick updates the platform to perform the rotation.
bool rotational_tick();

// rotational_state reads the current rotational state.
void rotational_state(rotational_state_t *state);
