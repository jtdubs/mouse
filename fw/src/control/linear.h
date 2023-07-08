#pragma once

#include <stdbool.h>

#include "control/pid.h"

extern float linear_start_distance;   // mm
extern float linear_target_distance;  // mm
extern float linear_target_speed;     // mm/s

void linear_init();
void linear_start(float distance /* mm */, bool stop);
bool linear_update();

void linear_wall_tune(float kp, float ki, float kd, float alpha);
void linear_angle_tune(float kp, float ki, float kd, float alpha);
