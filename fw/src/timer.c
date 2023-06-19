#include "timer.h"

#include <avr/interrupt.h>
#include <avr/io.h>

volatile bool timer_elapsed;

// timer_init initializes timer.
void timer_init() {
  // Setup a 1ms timer interrupt on TC1.
  TCCR0A = (1 << WGM01);               // Clear timer on compare match
  TCCR0B = (1 << CS00) | (1 << CS01);  // Use clk/64 prescaler (250kHz)
  OCR0A  = (F_CPU / 64 / (1000 + 1));  // 1000Hz = 1ms
  TIMSK0 = 1 << OCIE0A;                // Enable compare match interrupt
  TCNT0  = 0;                          // Reset the timer
}

ISR(TIMER0_COMPA_vect, ISR_BLOCK) {
  timer_elapsed = true;
}
