#include "platform/timer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <stdbool.h>

#include "platform/pin.h"

static volatile bool timer_elapsed;

// timer_init initializes timer.
void timer_init() {
  // Setup a 1ms timer interrupt on Timer0.
  TCCR0A = (1 << WGM01);               // Clear timer on compare match
  TCCR0B = (1 << CS00) | (1 << CS01);  // Use clk/64 prescaler (250kHz)
  OCR0A  = (F_CPU / 64 / (1000 + 1));  // 1000Hz = 1ms
  TIMSK0 = 1 << OCIE0A;                // Enable compare match interrupt
  TCNT0  = 0;                          // Reset the timer

  // Enable watchdog timer to reset the device if the timer interrupt fails.
  wdt_enable(WDTO_15MS);
}

// timer_wait waits for timer to elapse.
inline void timer_wait() {
  pin_clear(PROBE_0);

  // Wait with interrupts enabled for the next timer interrupt,
  // then disable interrupts and clear the timer.
  sei();
  while (!timer_elapsed) {};
  timer_elapsed = false;
  cli();

  wdt_reset();
  pin_set(PROBE_0);
}

ISR(TIMER0_COMPA_vect, ISR_BLOCK) {
  timer_elapsed = true;
}
