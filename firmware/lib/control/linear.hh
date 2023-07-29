//
// System: control
// Module: linear
//
// Purpose:
// - Handles linear motion plans.
//
// Design:
// - Drives a specified distance, and optionally stops at the end of the motion.
// - The mouse's velocity-over-time is a trapezoid with three phases:
//   - A constant acceleration phase to a cruising speed.
//   - A constant velocity phase.
//   - A constant decelleration phase to a stop (if requested).
//
#pragma once

#include <stdbool.h>

#if not defined(__AVR__)
#include <ostream>
#endif

namespace mouse::control::linear {

// PID tuning functions.
void TuneWallPID(float kp, float ki, float kd, float alpha);
void TuneAnglePID(float kp, float ki, float kd, float alpha);

#pragma pack(push, 1)
struct State {
  float target_position;  // mm
  float target_speed;     // mm/s
  float wall_error;
  bool  leds_prev_state;
};
#pragma pack(pop)

#if not defined(__AVR__)
[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const State *state) {
  o << "control::linear::State{" << std::endl;
  o << "  target_position: " << state->target_position << std::endl;
  o << "  target_speed: " << state->target_speed << std::endl;
  o << "  wall_error: " << state->wall_error << std::endl;
  o << "  leds_prev_state: " << state->leds_prev_state << std::endl;
  o << "}";
  return o;
}

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const State state) {
  return operator<<(o, &state);
}
#endif

}  // namespace mouse::control::linear
