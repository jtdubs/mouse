#include <avr/io.h>

#include "pin_impl.hh"

namespace mouse::platform::pin {

// Init initializes all pins.
void Init() {
  // Set all output pins to output mode.
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
  PORTB = 0;
  PORTC = 0;
  PORTD = 0;
}

}  // namespace mouse::platform::pin
