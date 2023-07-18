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

namespace linear {

// PID tuning functions.
void wall_tune(float kp, float ki, float kd, float alpha);
void angle_tune(float kp, float ki, float kd, float alpha);

}  // namespace linear