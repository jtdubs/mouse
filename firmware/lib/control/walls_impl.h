#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "walls.h"

// walls_init initializes the walls module.
void walls_init();

// walls_update reads the current sensor values and updates the wall state.
void walls_update();

// walls_error returns the "centering error" of the mouse, based on wall distances.
float walls_error();
