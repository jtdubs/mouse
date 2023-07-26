#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "walls.hh"

#if not defined(__AVR__)
#include <ostream>
#endif

namespace walls {

struct State {
  bool    left_present;
  bool    right_present;
  bool    forward_present;
  int16_t error_left;
  int16_t error_right;
};

// Init initializes the walls module.
void Init();

// update reads the current sensor values and updates the wall state.
void Update();

// error returns the "centering error" of the mouse, based on wall distances.
float error();

#if not defined(__AVR__)
std::ostream &operator<<(std::ostream &o, const State *state) {
  o << "walls::State{" << std::endl;
  o << "  left_present: " << state->left_present << std::endl;
  o << "  right_present: " << state->right_present << std::endl;
  o << "  forward_present: " << state->forward_present << std::endl;
  o << "  error_left: " << state->error_left << std::endl;
  o << "  error_right: " << state->error_right << std::endl;
  o << "}";
  return o;
}
#endif

}  // namespace walls
