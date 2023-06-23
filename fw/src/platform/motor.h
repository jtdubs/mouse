#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "platform/pin.h"

extern uint8_t motor_left_speed;
extern uint8_t motor_right_speed;
extern bool    motor_left_forward;
extern bool    motor_right_forward;

// motor_init initializes the motors.
void motor_init();

// set_left_motor_forward sets the direction of the left motor.
inline void set_left_motor_forward(bool forward) {
  motor_left_forward = forward;
  if (forward) {
    pin_clear(LEFT_DIR);
  } else {
    pin_set(LEFT_DIR);
  }
}

// set_right_motor_forward sets the direction of the right motor.
inline void set_right_motor_forward(bool forward) {
  motor_right_forward = forward;
  if (forward) {
    pin_set(RIGHT_DIR);
  } else {
    pin_clear(RIGHT_DIR);
  }
}

// set_left_motor_speed sets the speed of the left motor.
// NOTE: The speed range is [0, 255].
inline void set_left_motor_speed(uint8_t speed) {
  motor_left_speed = speed;
  OCR1A            = ((uint16_t)speed) << 1;
}

// set_right_motor_speed sets the speed of the right motor.
// NOTE: The speed range is [0, 255].
inline void set_right_motor_speed(uint8_t speed) {
  motor_right_speed = speed;
  OCR1B             = ((uint16_t)speed) << 1;
}
