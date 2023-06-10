#include "timer1.h"

#include <avr/interrupt.h>
#include <avr/io.h>

volatile bool timer1_elapsed = false;

// timer1_init initializes timer1.
void timer1_init() {
  // Board-specific fudge factor based on oscilloscope measurements.
  const uint16_t fudge = 62;
  // Setup a 2ms timer interrupt on TC1.
  TCCR1A  = 0;
  TCCR1B  = (1 << CS10);                   // Use internal clock (16Mhz)
  TCCR1B |= (1 << WGM12);                  // Clear timer on compare match
  OCR1A   = (F_CPU / (1000 + 1)) + fudge;  // 1000Hz = 1ms
  TIMSK1  = 1 << OCIE1A;                   // Enable compare match interrupt
  TCNT1   = 0;                             // Reset the timer
}

ISR(TIMER1_COMPA_vect) {
  timer1_elapsed = true;
}
