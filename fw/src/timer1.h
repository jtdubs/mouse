#pragma once

#include <stdbool.h>

#include "pin.h"

extern volatile bool ready;

void timer1_init();

inline void timer1_wait() {
  pin_clear(PROBE_0);
  while (!ready) {};
  ready = false;
  pin_set(PROBE_0);
}
