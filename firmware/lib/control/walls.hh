//
// System: control
// Module: walls
//
// Purpose:
// - Determine the presence of walls based on IR readings.
//
// Design:
// - Assumes sensor calibration has been performed.
// - Compares IR readings to the calibrated values.
//
#pragma once

#include <stdbool.h>
#include <stdint.h>

#if not defined(__AVR__)
#include <ostream>
#endif

namespace mouse::control::walls {

struct State {
  bool    left_present;
  bool    right_present;
  bool    forward_present;
  int16_t error_left;
  int16_t error_right;
};

// ControlLEDs determines whether or not this module controls the status LEDs.
// If enabled, the LEDs will reflect the presence or absence of walls.
void ControlLEDs(bool enabled);

// present returns the presence of walls on each side of the mouse.
void Present(bool& left, bool& right, bool& forward);

#if not defined(__AVR__)
[[maybe_unused]] static std::ostream &operator<<(std::ostream& o, const State* state) {
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

}  // namespace mouse::control::walls
