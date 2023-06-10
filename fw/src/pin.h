#pragma once

#include <avr/io.h>
#include <stdint.h>

// pin_t represents a pin on the atmega328p.
typedef struct {
  volatile uint8_t* port;
  volatile uint8_t* ddr;
  uint8_t           pin;
  uint8_t           mask;
} pin_t;

#define LED_BUILTIN ((pin_t){.port = &PORTB, .ddr = &DDRB, .pin = 5, .mask = _BV(5)})
#define PROBE_0 ((pin_t){.port = &PORTC, .ddr = &DDRB, .pin = 3, .mask = _BV(3)})
#define PROBE_1 ((pin_t){.port = &PORTC, .ddr = &DDRB, .pin = 4, .mask = _BV(4)})
#define PROBE_2 ((pin_t){.port = &PORTC, .ddr = &DDRB, .pin = 5, .mask = _BV(5)})

// pin_init initializes all pins.
void pin_init();

// pin_input sets the pin to input mode.
inline void pin_input(pin_t pin) {
  *pin.ddr &= ~pin.mask;
}

// pin_output sets the pin to output mode.
inline void pin_output(pin_t pin) {
  *pin.ddr |= pin.mask;
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
