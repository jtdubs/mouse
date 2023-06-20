#pragma once

#include <stdint.h>

extern uint16_t distance_left;
extern uint16_t distance_center;
extern uint16_t distance_right;

void mode_wall_enter();
void mode_wall_tick();
