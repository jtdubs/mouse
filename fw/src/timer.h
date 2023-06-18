#pragma once

#include <avr/interrupt.h>
#include <stdbool.h>

#include "pin.h"

extern volatile bool timer_elapsed;

// timer_init initializes timer.
void timer_init();

// timer_wait waits for timer to elapse.
inline void timer_wait() {
  pin_clear(PROBE_0);
  sei();
  while (!timer_elapsed) {};
  cli();
  timer_elapsed = false;
  pin_set(PROBE_0);
}
