//
// System: control
// Module: position
//
// Purpose:
// - Track the mouse's position and rotation via encoder updates.
//
#pragma once

#include <stdint.h>

namespace position {

// read reads the current position.
void read(float* distance, float* theta);

// tare reads the clears the current position.
void tare(float* distance, float* theta);

}
