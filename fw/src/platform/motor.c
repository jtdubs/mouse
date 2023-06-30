#include "motor.h"

#include <avr/io.h>

#include "platform/pin.h"

int16_t motor_power_left;
int16_t motor_power_right;

// motor_init initializes the motors.
void motor_init() {
  TCCR1A = _BV(COM1A1)   // Clear OC1A when up-counting, set when down-counting
         | _BV(COM1B1);  // Clear OCBA when up-counting, set when down-counting
  TCCR1B = _BV(WGM13)    // Phase and frequency correct PWM
         | _BV(CS10);    // Use internal clock (16Mhz)
  ICR1   = 510;          // ~16kHz PWM frequency (15MHz / 510 / 2)
  OCR1A  = 0;            // 0% duty cycle
  OCR1B  = 0;            // 0% duty cycle
  TIMSK1 = 0;            // Disable interrupts.
  TIFR0  = 0;            // Clear interrupt flags.
  TCNT1  = 0;            // Reset counter.

  motor_set(0, 0);
}

// motor_set sets the direction of the motors.
void motor_set(int16_t left, int16_t right) {
  motor_power_left   = left;
  motor_power_right  = right;
  bool left_forward  = left >= 0;
  bool right_forward = right >= 0;

  // set the direction of the motors
  pin_set2(LEFT_DIR, !left_forward);
  pin_set2(RIGHT_DIR, right_forward);

  // set the PWM duty cycle for each motor
  OCR1A = left_forward ? left : -left;
  OCR1B = right_forward ? right : -right;
}
