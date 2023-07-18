#include "pin_impl.hh"

namespace pin {

const pin_t RIGHT_DIR   = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(0)};
const pin_t LEFT_PWM    = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(1)};
const pin_t RIGHT_PWM   = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(2)};
const pin_t kLEDLeft    = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(3)};
const pin_t IR_LEDS     = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(4)};
const pin_t kLEDOnboard = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(5)};
const pin_t kProbeTick  = {.port = &PORTC, .ddr = &DDRC, .mask = _BV(3)};
const pin_t PROBE_PLAN  = {.port = &PORTC, .ddr = &DDRC, .mask = _BV(4)};
const pin_t PROBE_2     = {.port = &PORTC, .ddr = &DDRC, .mask = _BV(5)};
const pin_t kLEDRight   = {.port = &PORTD, .ddr = &DDRD, .mask = _BV(6)};
const pin_t LEFT_DIR    = {.port = &PORTD, .ddr = &DDRD, .mask = _BV(7)};

// Init initializes all pins.
void Init() {
  // Set all output pins to output mode.
  DDRB = (RIGHT_DIR.mask)     //
       | (LEFT_PWM.mask)      //
       | (RIGHT_PWM.mask)     //
       | (kLEDLeft.mask)      //
       | (IR_LEDS.mask)       //
       | (kLEDOnboard.mask);  //
  DDRC = (kProbeTick.mask)    //
       | (PROBE_PLAN.mask)    //
       | (PROBE_2.mask);      //
  DDRD = (kLEDRight.mask)     //
       | (LEFT_DIR.mask);     //

  // Everything starts out low.
  PORTB = 0;
  PORTC = 0;
  PORTD = 0;
}

}  // namespace pin
