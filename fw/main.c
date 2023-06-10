#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "pin.h"
#include "usart0.h"

char *message = "Hello, world!\n";

int main() {
  pin_init();
  usart0_init();
  sei();

  usart0_set_write_buffer((uint8_t *)message, 14);

  for (;;) {
    if (usart0_write_ready()) {
      usart0_write();
    }

    pin_toggle(LED_BUILTIN);
    _delay_ms(500);
  }

  return 0;
}
