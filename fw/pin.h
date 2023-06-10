#pragma once

#include <avr/io.h>
#include <stdint.h>

typedef struct {
  volatile uint8_t* port;
  uint8_t           pin;
  uint8_t           mask;
} pin_t;

#define LED_BUILTIN ((pin_t){.port = &PORTB, .pin = 5, .mask = _BV(5)})
#define PROBE_0 ((pin_t){.port = &PORTC, .pin = 3, .mask = _BV(3)})
#define PROBE_1 ((pin_t){.port = &PORTC, .pin = 4, .mask = _BV(4)})
#define PROBE_2 ((pin_t){.port = &PORTC, .pin = 5, .mask = _BV(5)})

void pin_init();

inline void pin_set(pin_t pin) {
  *pin.port |= pin.mask;
}

inline void pin_clear(pin_t pin) {
  *pin.port &= ~pin.mask;
}

inline void pin_toggle(pin_t pin) {
  *pin.port ^= pin.mask;
}
