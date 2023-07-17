#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "plan.hh"

namespace plan {

// init initializes the plan module.
void init();

// set_state sets the current plan state.
void set_state(state_t state);

// read reads the current plan.
void read(plan_t *plan);

}  // namespace plan
