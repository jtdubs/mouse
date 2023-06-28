#pragma once

#include <stdint.h>

// The measured motor speeds via encoder period in microseconds.
extern int16_t speed_measured_left;
extern int16_t speed_measured_right;

// The speeds setpoints via encoder period in microsecond
extern int16_t speed_setpoint_left;
extern int16_t speed_setpoint_right;

void speed_init();
void speed_update();

void speed_enable();
void speed_disable();

void speed_set_left(int16_t setpoint);
void speed_set_right(int16_t setpoint);