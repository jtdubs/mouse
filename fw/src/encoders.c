#include "encoders.h"

#include <avr/interrupt.h>
#include <avr/io.h>

uint16_t encoder_left;
uint16_t encoder_right;

void encoders_init() {
  EICRA = (1 << ISC00)   // INT0 on any logical change
        | (0 << ISC01)   // INT0 on any logical change
        | (1 << ISC10)   // INT1 on any logical change
        | (0 << ISC11);  // INT1 on any logical change
  EIMSK = (1 << INT0)    // Enable INT0
        | (1 << INT1);   // Enable INT1
}

// Left Encoder Clock
ISR(INT0_vect) {
  static uint8_t last_b = 0;

  uint8_t d = PORTD;
  uint8_t b = (d >> 4) & 1;
  uint8_t a = ((d >> 2) & 1) ^ b;

  if (a == last_b) {
    encoder_left++;
  } else {
    encoder_left--;
  }

  last_b = b;
}

// Right Encoder Clock
ISR(INT1_vect) {
  static uint8_t last_b = 0;

  uint8_t d = PORTD;
  uint8_t b = (d >> 5) & 1;
  uint8_t a = ((d >> 3) & 1) ^ b;

  if (a == last_b) {
    encoder_right++;
  } else {
    encoder_right--;
  }

  last_b = b;
}
