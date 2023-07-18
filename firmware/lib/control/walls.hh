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

namespace walls {

// led_control determines whether or not this module controls the status LEDs.
// If enabled, the LEDs will reflect the presence or absence of walls.
void led_control(bool enabled);

// present returns the presence of walls on each side of the mouse.
void present(bool& left, bool& right, bool& forward);

}  // namespace walls
