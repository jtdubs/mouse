#include "mode_error.h"

#include "mode.h"
#include "motor.h"
#include "pin.h"

uint8_t  error_code;
uint8_t  bit_index;
uint16_t counter;

void mode_error_tick() {
  // on for first 8 bits, and first 100ms of each bit
  bool on  = bit_index < 8 && counter < 20;
  bool bit = false;
  if (on) {
    bit = (error_code >> (7 - bit_index)) & 1;
  }
  pin_set2(LED_LEFT, on && bit);
  pin_set2(LED_RIGHT, on && !bit);

  counter = counter + 1;
  if (counter > 250) {
    counter   = 0;
    bit_index = (bit_index + 1) & 0xF;
  }
}

void mode_error(uint8_t error) {
  error_code = error;
  mode_set(MODE_ERROR);
}
