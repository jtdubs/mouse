#pragma once

#include <stdbool.h>

#include "linear.hh"

namespace linear {

#pragma pack(push, 1)
struct state_t {
  float target_position;  // mm
  float target_speed;     // mm/s
  float wall_error;
  bool  leds_prev_state;
};
#pragma pack(pop)

// init initializes the linear motion module.
void init();

// start starts a linear motion plan.
void start(float position /* mm */, bool stop);

// tick updates the platform to implement the plan.
// Returns true if the plan is complete.
bool tick();

// state reads the current linear state.
void read(state_t &s);

}  // namespace linear
