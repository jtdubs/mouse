#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "platform/pin.h"

extern int16_t motor_power_left;
extern int16_t motor_power_right;

// motor_init initializes the motors.
void motor_init();

// Sets the power of the motors.
void motor_set(int16_t left, int16_t right);
