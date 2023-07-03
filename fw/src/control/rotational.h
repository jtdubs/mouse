#pragma once

#include <stdbool.h>

extern float rotational_start_theta;   // radians
extern float rotational_target_theta;  // radians
extern float rotational_target_speed;  // radians/s
extern bool  rotational_direction;     // true = positive, false = negative

void rotational_init();
void rotational_start(float dtheta /* radians */);
bool rotational_update();
