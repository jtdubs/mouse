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

#if not defined(__AVR__)
[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const State *state) {
  o << "rotational::State{" << std::endl;
  o << "  start_theta: " << state->start_theta << std::endl;
  o << "  target_theta: " << state->target_theta << std::endl;
  o << "  direction: " << state->direction << std::endl;
  o << "}";
  return o;
}

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const State state) {
  return operator<<(o, &state);
}
#endif

}  // namespace rotational
