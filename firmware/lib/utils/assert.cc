#include "assert.hh"

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "firmware/platform/platform.hh"
#include "sim.h"

namespace mouse::assert {

namespace {
const char kHexTable[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};
}

// failed is called when an assertion fails.
void Failed(Module m, uint8_t n) {
  // disable interrupts and the watchdog (we are here forever!)
  cli();
  wdt_disable();

  uint16_t error_code = static_cast<uint8_t>(m) << 8 | n;

  // print the error code to the console (if in the simulator)
  SIM_CONSOLE_REG = static_cast<uint8_t>('A');
  SIM_CONSOLE_REG = static_cast<uint8_t>(':');
  SIM_CONSOLE_REG = static_cast<uint8_t>(kHexTable[error_code >> 12 & 0xF]);
  SIM_CONSOLE_REG = static_cast<uint8_t>(kHexTable[error_code >> 8 & 0xF]);
  SIM_CONSOLE_REG = static_cast<uint8_t>(kHexTable[error_code >> 4 & 0xF]);
  SIM_CONSOLE_REG = static_cast<uint8_t>(kHexTable[error_code >> 0 & 0xF]);
  SIM_CONSOLE_REG = static_cast<uint8_t>('\n');

  // disable all peripherals
  platform::motor::Set(0, 0);
  platform::pin::Clear(platform::pin::kLEDLeft);
  platform::pin::Clear(platform::pin::kLEDRight);
  platform::pin::Clear(platform::pin::kLEDOnboard);
  platform::pin::Clear(platform::pin::kIRLEDs);

  for (;;) {
    // blink out each bit in the error code
    for (uint8_t bit_index = 0; bit_index < 16; bit_index++) {
      bool bit = (error_code >> (15 - bit_index)) & 1;

      // on for 20ms, off for 230ms (determined by visual inspection)
      platform::pin::Set(bit ? platform::pin::kLEDLeft : platform::pin::kLEDRight);
      _delay_ms(20);
      platform::pin::Clear(bit ? platform::pin::kLEDLeft : platform::pin::kLEDRight);
      _delay_ms(230);

      if ((bit_index & 3) == 3) {
        _delay_ms(250);
      }
    }

    // wait a bit before repeating
    _delay_ms(2000);
  }
}

}  // namespace mouse::assert
