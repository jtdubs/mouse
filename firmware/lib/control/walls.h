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

// walls_led_control determines whether or not this module controls the status LEDs.
// If enabled, the LEDs will reflect the presence or absence of walls.
void walls_led_control(bool enabled);

// walls_present returns the presence of walls on each side of the mouse.
void walls_present(bool* left, bool* right, bool* forward);
