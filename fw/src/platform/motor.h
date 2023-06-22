#pragma once

#include <stdbool.h>

#include "platform/pin.h"

// motor_init initializes the motors.
void motor_init();

// set_left_motor_dir sets the direction of the left motor.
inline void set_left_motor_dir(bool forward) {
  if (forward) {
    pin_set(LEFT_DIR);
  } else {
    pin_clear(LEFT_DIR);
  }
}

// set_right_motor_dir sets the direction of the right motor.
inline void set_right_motor_dir(bool forward) {
  if (forward) {
    pin_set(RIGHT_DIR);
  } else {
    pin_clear(RIGHT_DIR);
  }
}

// set_left_motor_speed sets the speed of the left motor.
// NOTE: The speed range is [0, 255].
inline void set_left_motor_speed(uint8_t speed) {
  OCR1A = ((uint16_t)speed) << 1;
}

// set_right_motor_speed sets the speed of the right motor.
// NOTE: The speed range is [0, 255].
inline void set_right_motor_speed(uint8_t speed) {
  OCR1B = ((uint16_t)speed) << 1;
}
