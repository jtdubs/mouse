#pragma once

#include <stdbool.h>

#include "rotational.hh"

#if not defined(__AVR__)
#include <ostream>
#endif

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

#if not defined(__AVR__)
std::ostream &operator<<(std::ostream &o, const State *state) {
  o << "rotational::State{" << std::endl;
  o << "  start_theta: " << state->start_theta << std::endl;
  o << "  target_theta: " << state->target_theta << std::endl;
  o << "  direction: " << state->direction << std::endl;
  o << "}";
  return o;
}
#endif

}  // namespace rotational
