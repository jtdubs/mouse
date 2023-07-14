//
// System: control
// Module: position
//
// Purpose:
// - Track the mouse's position and rotation via encoder updates.
//
#pragma once

#include <stdint.h>

// position_init initializes the position module.
void position_init();

// position_update updates the position based on encoder deltas.
void position_update();

// position_clear clears the position (basically a tare function).
void position_clear();

// position_read reads the current position.
void position_read(float* distance, float* theta);
