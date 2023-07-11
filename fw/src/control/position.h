//
// System: control
// Module: position
//
// Purpose:
// - Track the mouse's position and rotation via encoder updates.
//
#pragma once

#include <stdint.h>

// Position (via dead reckoning).
extern float position_distance;  // in mms
extern float position_theta;     // in radians

// position_init initializes the position module.
void position_init();

// position_update updates the position based on encoder deltas.
void position_update();

// position_clear clears the position (basically a tare function).
void position_clear();
