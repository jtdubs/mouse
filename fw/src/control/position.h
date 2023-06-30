#pragma once

#include <stdint.h>

// Measured motor speeds.
extern float position_distance;  // in mms
extern float position_theta;     // in radians

void position_init();
void position_read();
