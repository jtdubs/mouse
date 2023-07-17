#include "pin_impl.hh"

namespace pin {

const pin_t RIGHT_DIR   = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(0)};
const pin_t LEFT_PWM    = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(1)};
const pin_t RIGHT_PWM   = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(2)};
const pin_t LED_LEFT    = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(3)};
const pin_t IR_LEDS     = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(4)};
const pin_t LED_ONBOARD = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(5)};
const pin_t PROBE_TICK  = {.port = &PORTC, .ddr = &DDRC, .mask = _BV(3)};
const pin_t PROBE_PLAN  = {.port = &PORTC, .ddr = &DDRC, .mask = _BV(4)};
const pin_t PROBE_2     = {.port = &PORTC, .ddr = &DDRC, .mask = _BV(5)};
const pin_t LED_RIGHT   = {.port = &PORTD, .ddr = &DDRD, .mask = _BV(6)};
const pin_t LEFT_DIR    = {.port = &PORTD, .ddr = &DDRD, .mask = _BV(7)};

// init initializes all pins.
void init() {
  // Set all output pins to output mode.
  DDRB = (RIGHT_DIR.mask)     //
       | (LEFT_PWM.mask)      //
       | (RIGHT_PWM.mask)     //
       | (LED_LEFT.mask)      //
       | (IR_LEDS.mask)       //
       | (LED_ONBOARD.mask);  //
  DDRC = (PROBE_TICK.mask)    //
       | (PROBE_PLAN.mask)    //
       | (PROBE_2.mask);      //
  DDRD = (LED_RIGHT.mask)     //
       | (LEFT_DIR.mask);     //

  // Everything starts out low.
  PORTB = 0;
  PORTC = 0;
  PORTD = 0;
}

}  // namespace pin
