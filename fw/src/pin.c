#include "pin.h"

// pin_init initializes all pins.
void pin_init() {
  pin_output(LED_BUILTIN);
  pin_output(PROBE_0);
  pin_output(PROBE_1);
  pin_output(PROBE_2);
  pin_output(LEFT_DIR);
  pin_output(RIGHT_DIR);
  pin_output(LEFT_PWM);
  pin_output(RIGHT_PWM);

  pin_clear(LED_BUILTIN);
  pin_clear(PROBE_0);
  pin_clear(PROBE_1);
  pin_clear(PROBE_2);
  pin_clear(LEFT_DIR);
  pin_clear(RIGHT_DIR);
  pin_clear(LEFT_PWM);
  pin_clear(RIGHT_PWM);
}
