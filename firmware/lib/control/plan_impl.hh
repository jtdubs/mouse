#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "plan.hh"

namespace plan {

// init initializes the plan module.
void init();

// set_state sets the current plan state.
void set_state(State state);

// current gets the current plan.
plan_t current();

}  // namespace plan
