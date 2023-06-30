#include "rtc.h"

#include <avr/io.h>
#include <util/atomic.h>

static uint32_t rtc_overflow_count;

// rtc_init initializes the real-time clock.
void rtc_init() {
  TCCR2A = (0 << COM2A1)  // normal port operation
         | (0 << COM2A0)  // normal port operation
         | (0 << COM2B1)  // normal port operation
         | (0 << COM2B0)  // normal port operation
         | (0 << WGM21)   // disable PWM
         | (0 << WGM20);  // disable PWM

  TCCR2B = (0 << FOC2A)  // no force output compare
         | (0 << FOC2B)  // no force output compare
         | (0 << WGM22)  // disable PWM
         | (0 << CS22)   // use clk/32 prescaler (500kHz)
         | (1 << CS21)   // use clk/32 prescaler (500kHz)
         | (1 << CS20);  // use clk/32 prescaler (500kHz)

  TIMSK2 = (0 << OCIE2B)  // no compare match interrupt
         | (0 << OCIE2A)  // no compare match interrupt
         | (1 << TOIE2);  // enable overflow interrupt

  TIFR2 = (1 << OCF2B)  // clear compare match flag
        | (1 << OCF2A)  // clear compare match flag
        | (1 << TOV2);  // clear overflow flag

  TCNT2 = 0;  // reset the timer
}

// rtc_micros returns the number of microseconds since RTC initialization.
// NOTE: MUST BE CALLED FROM AN ATOMIC BLOCK!
uint32_t rtc_micros() {
  uint8_t low = TCNT2;
  if (TIFR2 & (1 << TOV2)) {
    // overflow flag is set, but the interrupt hasn't run, so deal with it manually.
    rtc_overflow_count++;
    TIFR2 |= (1 << TOV2);  // clear overflow flag
    // and resample the counter, as the overflow may have happened just after the original sample.
    low = TCNT2;
  }
  return (rtc_overflow_count << 9) | (low << 1);
}

ISR(TIMER2_OVF_vect, ISR_BLOCK) {
  rtc_overflow_count++;
}
