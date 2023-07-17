#pragma once

#include <stdint.h>

#include "position.h"

// position_init initializes the position module.
void position_init();

// position_update updates the position based on encoder deltas.
void position_update();
