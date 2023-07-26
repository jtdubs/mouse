#pragma once

#include <stdbool.h>

#include "linear.hh"

#if not defined(__AVR__)
#include <ostream>
#endif

namespace linear {

#pragma pack(push, 1)
struct State {
  float target_position;  // mm
  float target_speed;     // mm/s
  float wall_error;
  bool  leds_prev_state;
};
#pragma pack(pop)

// Init initializes the linear motion module.
void Init();

// start starts a linear motion plan.
void Start(float position /* mm */, bool stop);

// Tick updates the platform to implement the plan.
// Returns true if the plan is complete.
bool Tick();

// state reads the current linear state.
void Read(State &s);

#if not defined(__AVR__)
std::ostream &operator<<(std::ostream &o, const State *state) {
  o << "linear::State{" << std::endl;
  o << "  target_position: " << state->target_position << std::endl;
  o << "  target_speed: " << state->target_speed << std::endl;
  o << "  wall_error: " << state->wall_error << std::endl;
  o << "  leds_prev_state: " << state->leds_prev_state << std::endl;
  o << "}";
  return o;
}
#endif

}  // namespace linear
