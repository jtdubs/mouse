#pragma once

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

// pin_t represents a pin on the atmega328p.
typedef struct {
  volatile uint8_t* port;
  volatile uint8_t* ddr;
  uint8_t           pin;
  uint8_t           mask;
} pin_t;

// clang-format off
#define RIGHT_DIR   ((pin_t){.port = &PORTB, .ddr = &DDRB, .pin = 0, .mask = _BV(0)})
#define LEFT_PWM    ((pin_t){.port = &PORTB, .ddr = &DDRB, .pin = 1, .mask = _BV(1)})
#define RIGHT_PWM   ((pin_t){.port = &PORTB, .ddr = &DDRB, .pin = 2, .mask = _BV(2)})
#define LED_LEFT    ((pin_t){.port = &PORTB, .ddr = &DDRB, .pin = 3, .mask = _BV(3)})
#define IR_LEDS     ((pin_t){.port = &PORTB, .ddr = &DDRB, .pin = 4, .mask = _BV(4)})
#define LED_BUILTIN ((pin_t){.port = &PORTB, .ddr = &DDRB, .pin = 5, .mask = _BV(5)})
#define PROBE_0     ((pin_t){.port = &PORTC, .ddr = &DDRC, .pin = 3, .mask = _BV(3)})
#define PROBE_1     ((pin_t){.port = &PORTC, .ddr = &DDRC, .pin = 4, .mask = _BV(4)})
#define PROBE_2     ((pin_t){.port = &PORTC, .ddr = &DDRC, .pin = 5, .mask = _BV(5)})
#define LED_RIGHT   ((pin_t){.port = &PORTD, .ddr = &DDRD, .pin = 6, .mask = _BV(6)})
#define LEFT_DIR    ((pin_t){.port = &PORTD, .ddr = &DDRD, .pin = 7, .mask = _BV(7)})
// clang-format on

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
