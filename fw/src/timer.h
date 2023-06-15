#pragma once

#include <stdbool.h>

#include "pin.h"

extern volatile bool timer_elapsed;

// timer_init initializes timer.
void timer_init();

// timer_wait waits for timer to elapse.
inline void timer_wait() {
  pin_clear(PROBE_0);
  while (!timer_elapsed) {};
  timer_elapsed = false;
  pin_set(PROBE_0);
}
