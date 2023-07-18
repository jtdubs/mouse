#pragma once

#include <stdbool.h>

#include "rotational.hh"

namespace rotational {

#pragma pack(push, 1)
struct state_t {
  float start_theta;   // radians
  float target_theta;  // radians
  bool  direction;     // true = positive, false = negative
};
#pragma pack(pop)

// init initializes the rotational motion module.
void init();

// start starts a rotational motion.
void start(float dtheta /* radians */);

// tick updates the platform to perform the rotation.
bool tick();

// state reads the current rotational state.
void read(state_t &state);

}  // namespace rotational
