//
// System: platform
// Module: pin
//
// Purpose:
// - Define mapping between pins and peripherals.
// - Provides functions for managing pin state (set/Clear/toggle).
//
#pragma once

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

namespace pin {

// pin_t represents an output pin on the atmega328p.
struct pin_t {
  volatile uint8_t* port;
  volatile uint8_t* ddr;
  uint8_t           mask;
};

// motors pins
extern const pin_t RIGHT_DIR;
extern const pin_t LEFT_DIR;
extern const pin_t LEFT_PWM;
extern const pin_t RIGHT_PWM;

// led pins
extern const pin_t kLEDLeft;
extern const pin_t kLEDRight;
extern const pin_t kLEDOnboard;
extern const pin_t IR_LEDS;

// probe pins
extern const pin_t kProbeTick;
extern const pin_t PROBE_PLAN;
extern const pin_t PROBE_2;

// Set sets the value of the pin.
inline void Set(pin_t pin, bool on) {
  if (on) {
    *pin.port |= pin.mask;
  } else {
    *pin.port &= ~pin.mask;
  }
}

// set sets the pin to high.
inline void Set(pin_t pin) {
  *pin.port |= pin.mask;
}

// Clear sets the pin to low.
inline void Clear(pin_t pin) {
  *pin.port &= ~pin.mask;
}

// toggle toggles the pin.
inline void toggle(pin_t pin) {
  *pin.port ^= pin.mask;
}

// is_set checks if the pin is set.
inline bool is_set(pin_t pin) {
  return *pin.port & pin.mask;
}

}  // namespace pin
