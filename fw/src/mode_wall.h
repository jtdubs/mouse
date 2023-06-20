#pragma once

#include <stdint.h>

extern uint8_t distance_left;
extern uint8_t distance_center;
extern uint8_t distance_right;

void mode_wall_enter();
void mode_wall_tick();
