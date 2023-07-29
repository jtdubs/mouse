//
// System: control
// Module: position
//
// Purpose:
// - Track the mouse's position and rotation via encoder updates.
//
#pragma once

#include <stdint.h>

namespace mouse::control::position {

// read reads the current position.
void Read(float& distance, float& theta);

// tare reads the clears the current position.
void Tare(float& distance, float& theta);

}  // namespace mouse::control::position
