#include "selector.h"

#include <stdbool.h>
#include <util/atomic.h>

#include "platform/adc.h"

// The proposed value (from the DIP switches) and the button state.
static uint8_t value;
static bool    button_pressed;

// Voltage thresholds for the DIP switches.
static const uint8_t SelectorThresholds[16] = {21,  42,  60,  77,  91,  102, 112, 123,
                                               133, 139, 144, 150, 156, 160, 163, 255};

uint8_t selector_update() {
  // Read the selector voltage.
  uint8_t v;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    v = adc_selector >> 2;
  }

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
