#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "platform/pin.h"

extern uint8_t motor_power_left;
extern uint8_t motor_power_right;
extern bool    motor_forward_left;
extern bool    motor_forward_right;

// motor_init initializes the motors.
void motor_init();

// Sets the direction of the motor.
void motor_set_forward_left(bool forward);
void motor_set_forward_right(bool forward);

// Sets the power of the motor.
void motor_set_power_left(uint8_t power);
void motor_set_power_right(uint8_t power);
