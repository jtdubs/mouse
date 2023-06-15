#pragma once

#include <stdbool.h>

#include "pin.h"

extern volatile bool timer0_elapsed;

// timer0_init initializes timer0.
void timer0_init();

// timer0_wait waits for timer0 to elapse.
inline void timer0_wait() {
  pin_clear(PROBE_0);
  while (!timer0_elapsed) {};
  timer0_elapsed = false;
  pin_set(PROBE_0);
}
