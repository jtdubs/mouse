#pragma once

#include <stdbool.h>

#include "linear.hh"

namespace linear {

// Init initializes the linear motion module.
void Init();

// start starts a linear motion plan.
void Start(float position /* mm */, bool stop);

// Tick updates the platform to implement the plan.
// Returns true if the plan is complete.
bool Tick();

// state reads the current linear state.
void Read(State &s);

}  // namespace linear
