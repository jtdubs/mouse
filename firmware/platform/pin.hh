//
// System: platform
// Module: pin
//
// Purpose:
// - Define mapping between pins and peripherals.
// - Provides functions for managing pin state (set/Clear/toggle).
//
#pragma once

#include <stdbool.h>
#include <stdint.h>

namespace mouse::platform::pin {

// Pin represents an output pin on the atmega328p.
struct Pin {
  volatile uint8_t* port;
  volatile uint8_t* ddr;
  uint8_t           mask;
};

// motors pins
extern const Pin kRightDir;
extern const Pin kLeftDir;
extern const Pin kLeftPWM;
extern const Pin kRightPWM;

// led pins
extern const Pin kLEDLeft;
extern const Pin kLEDRight;
extern const Pin kLEDOnboard;
extern const Pin kIRLEDs;

// probe pins
extern const Pin kProbeTick;
extern const Pin kProbePlan;
extern const Pin kProbeUnused;

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
