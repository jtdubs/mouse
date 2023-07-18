#pragma once

#include <stdbool.h>

#include "rotational.hh"

namespace rotational {

#pragma pack(push, 1)
struct State {
  float start_theta;   // radians
  float target_theta;  // radians
  bool  direction;     // true = positive, false = negative
};
#pragma pack(pop)

// Init initializes the rotational motion module.
void Init();

// start starts a rotational motion.
void Start(float dtheta /* radians */);

// Tick updates the platform to perform the rotation.
bool Tick();

// state reads the current rotational state.
void Read(State &state);

}  // namespace rotational
