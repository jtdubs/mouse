#include "assert.hh"

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "firmware/platform/motor.hh"
#include "firmware/platform/pin.hh"
#include "sim.h"

namespace assert {

namespace {
const char hex_table[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};
}

// failed is called when an assertion fails.
void failed(uint32_t error_code) {
  // disable interrupts and the watchdog (we are here forever!)
  cli();
  wdt_disable();

  // print the error code to the console (if in the simulator)
  SIM_CONSOLE_REG = (uint8_t)'A';
  SIM_CONSOLE_REG = (uint8_t)':';
  SIM_CONSOLE_REG = (uint8_t)hex_table[error_code >> 28 & 0xF];
  SIM_CONSOLE_REG = (uint8_t)hex_table[error_code >> 24 & 0xF];
  SIM_CONSOLE_REG = (uint8_t)hex_table[error_code >> 20 & 0xF];
  SIM_CONSOLE_REG = (uint8_t)hex_table[error_code >> 16 & 0xF];
  SIM_CONSOLE_REG = (uint8_t)hex_table[error_code >> 12 & 0xF];
  SIM_CONSOLE_REG = (uint8_t)hex_table[error_code >> 8 & 0xF];
  SIM_CONSOLE_REG = (uint8_t)hex_table[error_code >> 4 & 0xF];
  SIM_CONSOLE_REG = (uint8_t)hex_table[error_code >> 0 & 0xF];
  SIM_CONSOLE_REG = (uint8_t)'\n';

  // disable all peripherals
  motor::set(0, 0);
  pin::clear(pin::LED_LEFT);
  pin::clear(pin::LED_RIGHT);
  pin::clear(pin::LED_ONBOARD);
  pin::clear(pin::IR_LEDS);

  for (;;) {
    // blink out each bit in the error code
    for (uint8_t bit_index = 0; bit_index < 32; bit_index++) {
      bool bit = (error_code >> (31 - bit_index)) & 1;

      // on for 20ms, off for 230ms (determined by visual inspection)
      pin::set(bit ? pin::LED_LEFT : pin::LED_RIGHT);
      _delay_ms(20);
      pin::clear(bit ? pin::LED_LEFT : pin::LED_RIGHT);
      _delay_ms(230);

      if ((bit_index & 3) == 3) {
        _delay_ms(250);
      }
    }

    // wait a bit before repeating
    _delay_ms(2000);
  }
}

}  // namespace assert
