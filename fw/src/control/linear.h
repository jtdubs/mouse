#pragma once

#include <stdbool.h>

extern float linear_start_distance;   // mm
extern float linear_target_distance;  // mm
extern float linear_start_speed;      // rpm
extern float linear_target_speed;     // rpm
extern float linear_acceleration;     // rpm/tick
extern float linear_brake_distance;   // mm
extern float linear_coast_distance;   // mm

void linear_start(float distance /* mm */, bool coast);
bool linear_update();
