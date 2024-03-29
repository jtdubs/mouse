#include <avr/io.h>
#include <util/atomic.h>

#include "rtc_impl.hh"

namespace mouse::platform::rtc {

namespace {
// overflow_count is the number of times the timer has overflowed.
uint32_t overflow_count_;
}  // namespace

// Init initializes the real-time clock.
void Init() {
  TCCR2A = 0;                      // Normal port operation
  TCCR2B = _BV(CS21) | _BV(CS20);  // use clk/32 prescaler (500kHz)
  TIMSK2 = _BV(TOIE2);             // enable overflow interrupt
  TIFR2  = _BV(OCF2B)              // Clear compare match flag
        | _BV(OCF2A)               // Clear compare match flag
        | _BV(TOV2);               // Clear overflow flag
  TCNT2 = 0;                       // reset the timer
}

// micros returns the number of microseconds since RTC initialization.
uint32_t Micros() {
  uint32_t result;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    uint8_t low = TCNT2;
    if (TIFR2 & _BV(TOV2)) {
      // overflow flag is set, but the interrupt hasn't run, so deal with it manually.
      overflow_count_++;
      TIFR2 |= _BV(TOV2);  // Clear overflow flag
      // and resample the counter, as the overflow may have happened just after the original sample.
      low = TCNT2;
    }
    result = (overflow_count_ << 9) | (low << 1);
  }

  return result;
}

ISR(TIMER2_OVF_vect, ISR_BLOCK) {
  overflow_count_++;
}

}  // namespace mouse::platform::rtc
