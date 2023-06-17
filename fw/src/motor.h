#pragma once

#include <assert.h>
#include <stdbool.h>

#include "pin.h"

void motor_init();

inline void set_left_motor_dir(bool forward) {
  if (forward) {
    pin_set(LEFT_DIR);
  } else {
    pin_clear(LEFT_DIR);
  }
}

inline void set_right_motor_dir(bool forward) {
  if (forward) {
    pin_set(RIGHT_DIR);
  } else {
    pin_clear(RIGHT_DIR);
  }
}

inline void set_left_motor_speed(uint8_t speed) {
  OCR1A = ((uint16_t)speed) << 1;
}

inline void set_right_motor_speed(uint8_t speed) {
  OCR1B = ((uint16_t)speed) << 1;
}
