#include "assert.h"

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "platform/motor.h"
#include "platform/pin.h"

// _assert_failed is called when an assertion fails.
void _assert_failed(uint32_t error_code) {
  // disable interrupts and the watchdog (we are here forever!)
  cli();
  wdt_disable();

  // disable all peripherals
  motor_set(0, 0);
  pin_clear(LED_BUILTIN);
  pin_clear(LED_LEFT);
  pin_clear(LED_RIGHT);
  pin_clear(IR_LEDS);

  for (;;) {
    // blink out each bit in the error code
    for (uint8_t bit_index = 0; bit_index < 32; bit_index++) {
      bool bit = (error_code >> (31 - bit_index)) & 1;

      // on for 20ms, off for 230ms (determined by visual inspection)
      pin_set(bit ? LED_LEFT : LED_RIGHT);
      _delay_ms(20);
      pin_clear(bit ? LED_LEFT : LED_RIGHT);
      _delay_ms(230);

      if ((bit_index & 3) == 3) {
        _delay_ms(250);
      }
    }

    // wait a bit before repeating
    _delay_ms(2000);
  }
}
