#include "modes/mode_error.h"

#include "modes/mode.h"
#include "platform/motor.h"
#include "platform/pin.h"

// The error code and associated blink pattern state.
uint8_t  error_code;
uint8_t  bit_index;
uint16_t counter;

void mode_error_tick() {
  // Blink for first 8 bits, and first 20ms of each blink period.
  bool on = bit_index < 8 && counter < 20;

  // Determine the current bit value to blink (starts with high bit).
  bool bit = (error_code >> (7 - bit_index)) & 1;

  // Left for 1, right for 0.
  pin_set2(LED_LEFT, on && bit);
  pin_set2(LED_RIGHT, on && !bit);

  // Every 250ms, advance to the next bit.
  // NOTE: We wrap at 16-bits even though there are only 8,
  // as the last 8 bits are the pause between blink patterns.
  counter = counter + 1;
  if (counter > 250) {
    counter   = 0;
    bit_index = (bit_index + 1) & 0xF;
  }
}

// mode_error sets the error code, and enters the error mode.
void mode_error(uint8_t error) {
  error_code = error;
  mode_set(MODE_ERROR);
}
