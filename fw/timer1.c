#include "timer1.h"

#include <avr/interrupt.h>
#include <avr/io.h>

volatile bool ready = false;

void timer1_init() {
  // Board-specific fudge factor based on oscilloscope measurements.
  const uint16_t fudge = 62;
  // Setup a 2ms timer interrupt on TC1.
  TCCR1A  = 0;
  TCCR1B  = (1 << CS10);                  // Use internal clock (16Mhz)
  TCCR1B |= (1 << WGM12);                 // Clear timer on compare match
  OCR1A   = (F_CPU / (500 + 1)) + fudge;  // 500Hz = 2ms
  TIMSK1  = 1 << OCIE1A;                  // Enable compare match interrupt
  TCNT1   = 0;                            // Reset the timer
}

ISR(TIMER1_COMPA_vect) {
  ready = true;
}
