#pragma once

namespace mouse::platform::pin {

// Pin represents an output pin on the atmega328p.
struct Pin {
  volatile uint8_t* port;
  volatile uint8_t* ddr;
  uint8_t           mask;
};

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

void Init();

// Set sets the value of the pin.
inline void Set(Pin pin, bool on) {
  if (on) {
    *pin.port |= pin.mask;
  } else {
    *pin.port &= ~pin.mask;
  }
}

// set sets the pin to high.
inline void Set(Pin pin) {
  *pin.port |= pin.mask;
}

// Clear sets the pin to low.
inline void Clear(Pin pin) {
  *pin.port &= ~pin.mask;
}

// toggle toggles the pin.
inline void Toggle(Pin pin) {
  *pin.port ^= pin.mask;
}

// IsSet checks if the pin is set.
inline bool IsSet(Pin pin) {
  return *pin.port & pin.mask;
}

}  // namespace mouse::platform::pin
