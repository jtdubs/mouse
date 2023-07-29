#include "pin_impl.hh"

namespace mouse::platform::pin {

// TODO(justindubs): 4809 impl
const Pin kRightDir    = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(0)};
const Pin kLeftPWM     = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(1)};
const Pin kRightPWM    = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(2)};
const Pin kLEDLeft     = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(3)};
const Pin kIRLEDs      = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(4)};
const Pin kLEDOnboard  = {.port = &PORTB, .ddr = &DDRB, .mask = _BV(5)};
const Pin kProbeTick   = {.port = &PORTC, .ddr = &DDRC, .mask = _BV(3)};
const Pin kProbePlan   = {.port = &PORTC, .ddr = &DDRC, .mask = _BV(4)};
const Pin kProbeUnused = {.port = &PORTC, .ddr = &DDRC, .mask = _BV(5)};
const Pin kLEDRight    = {.port = &PORTD, .ddr = &DDRD, .mask = _BV(6)};
const Pin kLeftDir     = {.port = &PORTD, .ddr = &DDRD, .mask = _BV(7)};

// Init initializes all pins.
void Init() {
  // Set all output pins to output mode.
  // TODO(justindubs): 4809 impl
  DDRB = (kRightDir.mask)      //
       | (kLeftPWM.mask)       //
       | (kRightPWM.mask)      //
       | (kLEDLeft.mask)       //
       | (kIRLEDs.mask)        //
       | (kLEDOnboard.mask);   //
  DDRC = (kProbeTick.mask)     //
       | (kProbePlan.mask)     //
       | (kProbeUnused.mask);  //
  DDRD = (kLEDRight.mask)      //
       | (kLeftDir.mask);      //

  // Everything starts out low.
  // TODO(justindubs): 4809 impl
  PORTB = 0;
  PORTC = 0;
  PORTD = 0;
}

}  // namespace mouse::platform::pin
