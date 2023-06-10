#pragma once

#include <stdbool.h>

#include "pin.h"

extern volatile bool timer1_elapsed;

// timer1_init initializes timer1.
void timer1_init();

// timer1_wait waits for timer1 to elapse.
inline void timer1_wait() {
  pin_clear(PROBE_0);
  while (!timer1_elapsed) {};
  timer1_elapsed = false;
  pin_set(PROBE_0);
}
