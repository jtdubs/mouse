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

extern float rotational_start_theta;   // radians
extern float rotational_target_theta;  // radians
extern bool  rotational_direction;     // true = positive, false = negative

// rotational_init initializes the rotational motion module.
void rotational_init();

// rotational_start starts a rotational motion plan.
void rotational_start(float dtheta /* radians */);

// rotational_tick updates the platform to perform the rotation.
bool rotational_tick();
