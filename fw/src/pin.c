#include "pin.h"

// pin_init initializes all pins.
void pin_init() {
  pin_output(LED_BUILTIN);
  pin_output(PROBE_0);
  pin_output(PROBE_1);
  pin_output(PROBE_2);

  pin_clear(LED_BUILTIN);
  pin_clear(PROBE_0);
  pin_clear(PROBE_1);
  pin_clear(PROBE_2);
}
