#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "walls.hh"

namespace walls {

struct state_t {
  bool    left_present;
  bool    right_present;
  bool    forward_present;
  int16_t error_left;
  int16_t error_right;
};

// init initializes the walls module.
void init();

// update reads the current sensor values and updates the wall state.
void update();

// error returns the "centering error" of the mouse, based on wall distances.
float error();

}  // namespace walls
