#include "platform/timer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stdbool.h>

#include "platform/pin.h"

static volatile bool timer_elapsed;

// timer_init initializes timer.
void timer_init() {
  // Setup a 10ms timer interrupt on Timer0.
  TCCR0A = (0 << COM0A1)  // Normal port operation
         | (0 << COM0A0)  // Normal port operation
         | (0 << COM0B1)  // Normal port operation
         | (0 << COM0B0)  // Normal port operation
         | (1 << WGM01)   // Clear timer on OCRA match
         | (0 << WGM00);  // Clear timer on OCRA match

  TCCR0B = (0 << FOC0A)  // No force output compare
         | (0 << FOC0B)  // No force output compare
         | (0 << WGM02)  // Clear timer on OCRA match
         | (1 << CS02)   // Use clk/1024 prescaler (15.625kHz)
         | (0 << CS01)   // Use clk/1024 prescaler (15.625kHz)
         | (1 << CS00);  // Use clk/1024 prescaler (15.625kHz)

  OCR0A = (F_CPU / 1024 / (200 + 1)) + 1;  // 100Hz = 10ms
  OCR0B = OCR0A - 4;                       // 250us before OCR0A

  TIMSK0 = (1 << OCIE0B)  // Enable OCR0B match interrupt
         | (1 << OCIE0A)  // Enable OCR0A match interrupt
         | (0 << TOIE0);  // Disable overflow interrupt

  TCNT0 = 0;  // Reset the timer

  // Enable watchdog timer to reset the device if the timer interrupt fails.
  wdt_enable(WDTO_15MS);

  // Enable sleeping
  // set_sleep_mode(SLEEP_MODE_IDLE);
}

// timer_wait waits for timer to elapse.
inline void timer_wait() {
  pin_clear(PROBE_TICK);

  // Wait with interrupts enabled for the next timer interrupt,
  // then disable interrupts and clear the timer.
  while (!timer_elapsed) {
    // sleep_mode();
  };
  timer_elapsed = false;

  wdt_reset();
  pin_set(PROBE_TICK);
}

ISR(TIMER0_COMPA_vect, ISR_BLOCK) {
  timer_elapsed = true;
}

ISR(TIMER0_COMPB_vect, ISR_BLOCK) {
  // Start performing ADC conversion
  ADCSRA |= (1 << ADSC);
}
