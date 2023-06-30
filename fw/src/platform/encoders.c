#include "encoders.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "platform/pin.h"
#include "platform/rtc.h"

// Encoder counts.
int32_t encoders_left;
int32_t encoders_right;

// Changes to encoder counts since the last update.
int8_t encoders_left_delta;
int8_t encoders_right_delta;

// encoders_init initializes the encoders.
void encoders_init() {
  EICRA = _BV(ISC00)   // Trigger INT0 on any logical change
        | _BV(ISC10);  // Trigger INT1 on any logical change
  EIMSK = _BV(INT0)    // Enable INT0
        | _BV(INT1);   // Enable INT1
}

// encoders_update applies changes since the last update.
// NOTE: MUST BE CALLED FROM AN ATOMIC BLOCK!
void encoders_update() {
  encoders_left        += (int32_t)encoders_left_delta;
  encoders_right       += (int32_t)encoders_right_delta;
  encoders_left_delta   = 0;
  encoders_right_delta  = 0;
}

// Left Encoder Clock
ISR(INT0_vect, ISR_BLOCK) {
  pin_toggle(PROBE_1);

  static uint8_t left_last_b = 0;

  // Read the encoder pins (b and clk) and calculate a.
  uint8_t d   = PIND;
  uint8_t b   = (d >> 4) & 1;
  uint8_t clk = ((d >> 2) & 1);
  uint8_t a   = clk ^ b;

  // Update the encoder count based on rotation direction.
  if (a == left_last_b) {
    encoders_left_delta++;
  } else {
    encoders_left_delta--;
  }

  left_last_b = b;
}

// Right Encoder Clock
ISR(INT1_vect, ISR_BLOCK) {
  pin_toggle(PROBE_2);

  static uint8_t right_last_b = 0;

  // Read the encoder pins (b and clk) and calculate a.
  uint8_t d   = PIND;
  uint8_t b   = (d >> 5) & 1;
  uint8_t clk = ((d >> 3) & 1);
  uint8_t a   = clk ^ b;

  // Update the encoder count based on rotation direction.
  if (a == right_last_b) {
    encoders_right_delta--;
  } else {
    encoders_right_delta++;
  }

  right_last_b = b;
}
