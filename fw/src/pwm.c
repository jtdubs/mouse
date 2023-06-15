#include "pwm.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "pin.h"

void pwm_init() {
  TCCR1A = (1 << COM1A1)   // Clear OC1A when up-counting, set when down-counting
         | (1 << COM1B1);  // Clear OCBA when up-counting, set when down-counting
  TCCR1B = (1 << WGM13)    // Phase and frequency correct PWM
         | (1 << CS10);    // Use internal clock (16Mhz)
  ICR1   = 800;            // 10kHz PWM frequency
  OCR1A  = 0;              // 0% duty cycle
  OCR1B  = 0;              // 0% duty cycle
  TIMSK1 = 0;              // Disable interrupts.
  TIFR0  = 0;              // Clear interrupt flags.
  TCNT1  = 0;              // Reset counter.
}

void set_pwm_duty_cycle_a(uint8_t duty_cycle_percent) {
  OCR1A = duty_cycle_percent << 3;
}

void set_pwm_duty_cycle_b(uint8_t duty_cycle_percent) {
  OCR1B = duty_cycle_percent << 3;
}
