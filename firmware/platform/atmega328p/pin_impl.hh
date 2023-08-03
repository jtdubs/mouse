#pragma once

#include <avr/io.h>

namespace mouse::platform::pin {

// Pin represents an output pin on the atmega328p.
struct Pin {
  volatile uint8_t* port;
  uint8_t           mask;
};

const Pin kRightDir    = {.port = &PORTB, .mask = _BV(0)};  // D8
const Pin kLeftPWM     = {.port = &PORTB, .mask = _BV(1)};  // D9
const Pin kRightPWM    = {.port = &PORTB, .mask = _BV(2)};  // D10
const Pin kLEDLeft     = {.port = &PORTB, .mask = _BV(3)};  // D11
const Pin kIRLEDs      = {.port = &PORTB, .mask = _BV(4)};  // D12
const Pin kLEDOnboard  = {.port = &PORTB, .mask = _BV(5)};  // D13
const Pin kProbeTick   = {.port = &PORTC, .mask = _BV(3)};  // A3
const Pin kProbePlan   = {.port = &PORTC, .mask = _BV(4)};  // A4
const Pin kProbeUnused = {.port = &PORTC, .mask = _BV(5)};  // A5
const Pin kLEDRight    = {.port = &PORTD, .mask = _BV(6)};  // D6
const Pin kLeftDir     = {.port = &PORTD, .mask = _BV(7)};  // D7

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
