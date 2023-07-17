//
// System: control
// Module: position
//
// Purpose:
// - Track the mouse's position and rotation via encoder updates.
//
#pragma once

#include <stdint.h>

// position_read reads the current position.
void position_read(float* distance, float* theta);

// position_tare reads the clears the current position.
void position_tare(float* distance, float* theta);
