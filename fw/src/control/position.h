#pragma once

#include <stdint.h>

// Position (via dead reckoning).
extern float position_distance;  // in mms
extern float position_theta;     // in radians

void position_init();
void position_read();
void position_clear();
