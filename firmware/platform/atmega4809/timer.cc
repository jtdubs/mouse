#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <stdbool.h>
#include <stddef.h>
#include <util/atomic.h>

#include "adc_impl.hh"
#include "firmware/lib/utils/assert.hh"
#include "pin_impl.hh"
#include "timer_impl.hh"

namespace mouse::platform::timer {

namespace {
callback_t callbacks_[4];
uint8_t    callback_count_ = 0;
}  // namespace

void AddCallback(callback_t callback) {
  assert(assert::Module::Timer, 0, callback != NULL);
  assert(assert::Module::Timer, 1, callback_count_ < 4);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    callbacks_[callback_count_++] = callback;
  }
}

// Init initializes timer.
void Init() {
  // TODO(justindubs): 4809 impl
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
  // TODO(justindubs): 4809 impl
  wdt_reset();
  NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE) {
    for (uint8_t i = 0; i < callback_count_; i++) {
      callbacks_[i]();
    }
  }
}

ISR(TIMER0_COMPB_vect, ISR_BLOCK) {
  // TODO(justindubs): 4809 impl
  platform::adc::Sample();
}

}  // namespace mouse::platform::timer
