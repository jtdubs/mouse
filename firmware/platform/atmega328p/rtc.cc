#include "rtc.h"

#include <avr/io.h>
#include <util/atomic.h>

// rtc_overflow_count is the number of times the timer has overflowed.
static uint32_t rtc_overflow_count;

// rtc_init initializes the real-time clock.
void rtc_init() {
  TCCR2A = 0;                      // Normal port operation
  TCCR2B = _BV(CS21) | _BV(CS20);  // use clk/32 prescaler (500kHz)
  TIMSK2 = _BV(TOIE2);             // enable overflow interrupt
  TIFR2  = _BV(OCF2B)              // clear compare match flag
        | _BV(OCF2A)               // clear compare match flag
        | _BV(TOV2);               // clear overflow flag
  TCNT2 = 0;                       // reset the timer
}

// rtc_micros returns the number of microseconds since RTC initialization.
uint32_t rtc_micros() {
  uint32_t result;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    uint8_t low = TCNT2;
    if (TIFR2 & _BV(TOV2)) {
      // overflow flag is set, but the interrupt hasn't run, so deal with it manually.
      rtc_overflow_count++;
      TIFR2 |= _BV(TOV2);  // clear overflow flag
      // and resample the counter, as the overflow may have happened just after the original sample.
      low = TCNT2;
    }
    result = (rtc_overflow_count << 9) | (low << 1);
  }

  return result;
}

ISR(TIMER2_OVF_vect, ISR_BLOCK) {
  rtc_overflow_count++;
}
