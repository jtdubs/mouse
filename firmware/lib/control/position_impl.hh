#pragma once

#include <stdint.h>

#include "position.hh"

namespace position {

// init initializes the position module.
void init();

// update updates the position based on encoder deltas.
void update();

}
