#include "platform/pin.h"

// pin_init initializes all pins.
void pin_init() {
  DDRB = (RIGHT_DIR.mask)     //
       | (LEFT_PWM.mask)      //
       | (RIGHT_PWM.mask)     //
       | (LED_LEFT.mask)      //
       | (IR_LEDS.mask)       //
       | (LED_BUILTIN.mask);  //
  DDRC = (PROBE_0.mask)       //
       | (PROBE_1.mask)       //
       | (PROBE_2.mask);      //
  DDRD = (LED_RIGHT.mask)     //
       | (LEFT_DIR.mask);     //

  PORTB = 0;
  PORTC = 0;
  PORTD = 0;
}
