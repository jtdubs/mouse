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

// Whether or not each wall is present.
extern bool wall_left_present;
extern bool wall_right_present;
extern bool wall_forward_present;

// The left/right error vs. the calibrated values.
extern int16_t wall_error_left;
extern int16_t wall_error_right;

// walls_init initializes the walls module.
void walls_init();

// walls_led_control determines whether or not this module controls the status LEDs.
// If enabled, the LEDs will reflect the presence or absence of walls.
void walls_led_control(bool enabled);

// walls_update reads the current sensor values and updates the wall state.
void walls_update();
