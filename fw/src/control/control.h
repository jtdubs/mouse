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
// - Implments a "plan" provided by the higher-level modules, for example:
//   - A "linear motion" plan to drive a certain distance at a certain speed.
//   - A "rotational motion" plan to turn by a certain number of radians.
// - Provides feedback to the higher-level modules on the progress of the plan.
//
#pragma once

#include "control/plan.h"

// control_init initializes the control module.
void control_init();

// control_tick executes one tick of the control module.
void control_tick();

// control_report sends a control report.
uint8_t control_report(uint8_t *buffer, uint8_t len);
