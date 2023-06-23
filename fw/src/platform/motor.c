#include "motor.h"

#include <avr/io.h>

#include "platform/pin.h"

uint8_t motor_left_speed;
uint8_t motor_right_speed;
bool    motor_left_forward;
bool    motor_right_forward;

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

  set_left_motor_forward(true);
  set_right_motor_forward(true);
}
