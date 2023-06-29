#include "motor.h"

#include <avr/io.h>

#include "platform/pin.h"

uint16_t motor_power_left;
uint16_t motor_power_right;
bool     motor_forward_left;
bool     motor_forward_right;

// motor_init initializes the motors.
void motor_init() {
  TCCR1A = (1 << COM1A1)   // Clear OC1A when up-counting, set when down-counting
         | (1 << COM1B1);  // Clear OCBA when up-counting, set when down-counting
  TCCR1B = (1 << WGM13)    // Phase and frequency correct PWM
         | (1 << CS10);    // Use internal clock (16Mhz)
  ICR1   = 510;            // ~16kHz PWM frequency
  OCR1A  = 0;              // 0% duty cycle
  OCR1B  = 0;              // 0% duty cycle
  TIMSK1 = 0;              // Disable interrupts.
  TIFR0  = 0;              // Clear interrupt flags.
  TCNT1  = 0;              // Reset counter.

  motor_set_forward_left(true);
  motor_set_forward_right(true);
}

// set_left_motor_forward sets the direction of the left motor.
void motor_set_forward_left(bool forward) {
  motor_forward_left = forward;
  if (forward) {
    pin_clear(LEFT_DIR);
  } else {
    pin_set(LEFT_DIR);
  }
}

// set_right_motor_forward sets the direction of the right motor.
void motor_set_forward_right(bool forward) {
  motor_forward_right = forward;
  if (forward) {
    pin_set(RIGHT_DIR);
  } else {
    pin_clear(RIGHT_DIR);
  }
}

// set_left_motor_power sets the power of the left motor.
// NOTE: The power range is [0, 255].
void motor_set_power_left(uint16_t power) {
  motor_power_left = power;
  OCR1A            = power;
}

// set_right_motor_power sets the power of the right motor.
// NOTE: The power range is [0, 255].
void motor_set_power_right(uint16_t power) {
  motor_power_right = power;
  OCR1B             = power;
}
