#include "pin_impl.hh"

namespace mouse::platform::pin {

// Init initializes all pins.
void Init() {
  PORTA_DIRSET = 0b00001110;
  PORTB_DIRSET = 0b00000011;
  PORTC_DIRSET = 0b00000000;
  PORTD_DIRSET = 0b00000001;
  PORTE_DIRSET = 0b00001111;
  PORTF_DIRSET = 0b00010000;

  PORTA_OUTCLR = 0b00001110;
  PORTB_OUTCLR = 0b00000011;
  PORTC_OUTCLR = 0b00000000;
  PORTD_OUTCLR = 0b00000001;
  PORTE_OUTCLR = 0b00001111;
  PORTF_OUTCLR = 0b00010000;
}

}  // namespace mouse::platform::pin
