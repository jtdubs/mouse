#include "pin.h"

// pin_init initializes all pins.
void pin_init() {
  DDRB = (1 << RIGHT_DIR.pin) | (1 << LEFT_PWM.pin) | (1 << RIGHT_PWM.pin) | (1 << LED_BUILTIN.pin);
  DDRC = (1 << PROBE_0.pin) | (1 << PROBE_1.pin) | (1 << PROBE_2.pin);
  DDRD = (1 << USER_IO.pin) | (1 << LEFT_DIR.pin);

  PORTB = 0;
  PORTC = 0;
  PORTD = 0;
}
