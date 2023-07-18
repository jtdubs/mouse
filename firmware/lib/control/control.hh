//
// System: control
// Module: control
//
// Purpose:
// - The overall control loop for the mouse.
// - Drives the platform layer to drive all the mouse I/O.
//
// Design:
// - Executed by the platform's timer.
// - Implements a "plan" provided by the higher-level modules, for example:
//   - A "linear motion" plan to drive a certain distance at a certain speed.
//   - A "rotational motion" plan to turn by a certain number of radians.
// - Provides feedback to the higher-level modules on the progress of the plan.
//
#pragma once

namespace control {

// Init initializes the control module.
void Init();

// report sends a control report.
uint8_t GetReport(uint8_t *buffer, uint8_t len);

}  // namespace control
