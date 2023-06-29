#pragma once

#include <stdint.h>

extern uint8_t wall_distance_left;
extern uint8_t wall_distance_center;
extern uint8_t wall_distance_right;

void mode_wall_enter();
void mode_wall_tick();
