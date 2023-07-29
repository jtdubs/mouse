#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "plan.hh"

namespace mouse::control::plan {

// Init initializes the plan module.
void Init();

// SetState sets the current plan state.
void SetState(State state);

// current gets the current plan.
Plan Current();

}  // namespace mouse::control::plan
