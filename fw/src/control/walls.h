#pragma once

#include <stdbool.h>
#include <stdint.h>

extern bool wall_left_present;
extern bool wall_right_present;
extern bool wall_forward_present;

extern int16_t wall_error_left;
extern int16_t wall_error_right;

void walls_init();
void walls_led_control(bool enabled);
void walls_update();
