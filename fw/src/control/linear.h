#pragma once

#include <stdbool.h>

extern float linear_start_distance;   // mm
extern float linear_target_distance;  // mm
extern float linear_target_speed;     // rpm
extern bool  linear_braking;

void linear_init();
void linear_start(float distance /* mm */, bool stop);
bool linear_update();

void linear_set_pi_coefficients(float kp, float ki, float alpha);
