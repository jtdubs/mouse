#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "platform/pin.h"

extern uint8_t motor_speed_left;
extern uint8_t motor_speed_right;
extern bool    motor_forward_left;
extern bool    motor_forward_right;

// motor_init initializes the motors.
void motor_init();

// set_left_motor_forward sets the direction of the left motor.
inline void motor_set_forward_left(bool forward) {
  motor_forward_left = forward;
  if (forward) {
    pin_clear(LEFT_DIR);
  } else {
    pin_set(LEFT_DIR);
  }
}

// set_right_motor_forward sets the direction of the right motor.
inline void motor_set_forward_right(bool forward) {
  motor_forward_right = forward;
  if (forward) {
    pin_set(RIGHT_DIR);
  } else {
    pin_clear(RIGHT_DIR);
  }
}

// set_left_motor_speed sets the speed of the left motor.
// NOTE: The speed range is [0, 255].
inline void motor_set_speed_left(uint8_t speed) {
  motor_speed_left = speed;
  OCR1A            = ((uint16_t)speed) << 1;
}

// set_right_motor_speed sets the speed of the right motor.
// NOTE: The speed range is [0, 255].
inline void motor_set_speed_right(uint8_t speed) {
  motor_speed_right = speed;
  OCR1B             = ((uint16_t)speed) << 1;
}
