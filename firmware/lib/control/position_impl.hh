#pragma once

#include <stdint.h>

#include "position.hh"

namespace mouse::control::position {

// Init initializes the position module.
void Init();

// update updates the position based on encoder deltas.
void Update();

}  // namespace mouse::control::position
