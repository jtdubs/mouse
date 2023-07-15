#if defined(BOARD_NANO)

#include "timer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <stdbool.h>
#include <stddef.h>
#include <util/atomic.h>

#include "adc.h"
#include "pin.h"
#include "utils/assert.h"

static timer_callback_t timer_callback;

void timer_set_callback(timer_callback_t callback) {
  assert(ASSERT_TIMER + 0, callback != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    timer_callback = callback;
  }
}

// timer_init initializes timer.
void timer_init() {
  TCCR0A = _BV(WGM01);                      // Clear timer on OCRA match
  TCCR0B = _BV(CS02) | _BV(CS00);           // Use clk/1024 prescaler (15.625kHz)
  OCR0A  = (F_CPU / 1024 / (200 + 1)) + 1;  // 200Hz = 5ms
  OCR0B  = OCR0A - 15;                      // 256us before OCR0A
  TIMSK0 = _BV(OCIE0B)                      // Enable OCR0B match interrupt
         | _BV(OCIE0A);                     // Enable OCR0A match interrupt
  TCNT0 = 0;                                // Reset the timer

  // Enable watchdog timer to reset the device if the timer interrupt fails.
  wdt_enable(WDTO_15MS);
}

ISR(TIMER0_COMPA_vect, ISR_BLOCK) {
  wdt_reset();
  if (timer_callback) {
    NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE) {
      timer_callback();
    }
  }
}

ISR(TIMER0_COMPB_vect, ISR_BLOCK) {
  adc_sample();
}

#endif
