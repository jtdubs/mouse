#include <stdbool.h>
#include <util/atomic.h>

#include "adc_impl.hh"
#include "selector_impl.hh"

namespace selector {

namespace {
// The proposed value (from the DIP switches) and the button state.
uint8_t value;
bool    button_pressed;
}  // namespace

uint8_t Update() {
  // Read the selector voltage.
  uint16_t raw = adc::Read(adc::Channel::Selector);

  uint8_t v = raw >> 2;

  if (v > 180) {
    // If the button was just pressed, return the selected value.
    if (!button_pressed) {
      button_pressed = true;
      return value;
    }
  } else {
    // Otherwise, decode the selector voltage.
    button_pressed = false;
    for (int i = 0; i < 16; i++) {
      if (v < SelectorThresholds[i]) {
        value = 15 - i;
        break;
      }
    }
  }

  // Nothing was selected.
  return 0xFF;
}

}  // namespace selector
