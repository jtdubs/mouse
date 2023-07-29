#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "walls.hh"

namespace mouse::control::walls {

// Init initializes the walls module.
void Init();

// update reads the current sensor values and updates the wall state.
void Update();

// error returns the "centering error" of the mouse, based on wall distances.
float error();

}  // namespace mouse::control::walls
