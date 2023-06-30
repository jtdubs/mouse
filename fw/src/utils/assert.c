#include "assert.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include "platform/pin.h"

void _assert_failed(uint8_t error_code) {
  cli();

  // TODO: platform_reset()
  // TODO: control_disable()

  for (;;) {
    for (uint8_t bit_index = 0; bit_index < 8; bit_index++) {
      bool bit = (error_code >> (7 - bit_index)) & 1;

      pin_set(bit ? LED_LEFT : LED_RIGHT);
      _delay_ms(20);
      pin_clear(bit ? LED_LEFT : LED_RIGHT);
      _delay_ms(230);
    }
    _delay_ms(2000);
  }
}
