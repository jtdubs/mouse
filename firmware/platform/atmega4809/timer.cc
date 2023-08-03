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
  // TODO
}

}  // namespace mouse::platform::timer
