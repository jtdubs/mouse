//
// System: platform
// Module: pin
//
// Purpose:
// - Define mapping between pins and peripherals.
// - Provides functions for managing pin state (set/clear/toggle).
//
#pragma once

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

// pin_t represents an output pin on the atmega328p.
typedef struct {
  volatile uint8_t* port;
  volatile uint8_t* ddr;
  uint8_t           mask;
} pin_t;

// motors pins
extern const pin_t RIGHT_DIR;
extern const pin_t LEFT_DIR;
extern const pin_t LEFT_PWM;
extern const pin_t RIGHT_PWM;

// led pins
extern const pin_t LED_LEFT;
extern const pin_t LED_BUILTIN;
extern const pin_t LED_RIGHT;
extern const pin_t IR_LEDS;

// probe pins
extern const pin_t PROBE_TICK;
extern const pin_t PROBE_PLAN;
extern const pin_t PROBE_2;

// pin_init initializes all pins.
void pin_init();

// pin_read sets the value of the pin.
inline void pin_set2(pin_t pin, bool on) {
  if (on) {
    *pin.port |= pin.mask;
  } else {
    *pin.port &= ~pin.mask;
  }
}

// pin_set sets the pin to high.
inline void pin_set(pin_t pin) {
  *pin.port |= pin.mask;
}

// pin_clear sets the pin to low.
inline void pin_clear(pin_t pin) {
  *pin.port &= ~pin.mask;
}

// pin_toggle toggles the pin.
inline void pin_toggle(pin_t pin) {
  *pin.port ^= pin.mask;
}

// pin_is_set checks if the pin is set.
inline bool pin_is_set(pin_t pin) {
  return *pin.port & pin.mask;
}
