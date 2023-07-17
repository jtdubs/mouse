#include "timer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <stdbool.h>
#include <stddef.h>
#include <util/atomic.h>

#include "adc.h"
#include "firmware/lib/utils/assert.h"
#include "pin.h"

static timer_callback_t timer_callbacks[4];
static uint8_t          timer_callback_count = 0;

void timer_add_callback(timer_callback_t callback) {
  assert(ASSERT_TIMER + 0, callback != NULL);
  assert(ASSERT_TIMER + 1, timer_callback_count < 4);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    timer_callbacks[timer_callback_count++] = callback;
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
  NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE) {
    for (uint8_t i = 0; i < timer_callback_count; i++) {
      timer_callbacks[i]();
    }
  }
}

ISR(TIMER0_COMPB_vect, ISR_BLOCK) {
  adc_sample();
}
