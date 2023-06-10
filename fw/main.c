#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "pin.h"
#include "timer1.h"
#include "usart0.h"

char *message = "Hello, world!\n";

int main() {
  pin_init();
  usart0_init();
  timer1_init();
  usart0_set_write_buffer((uint8_t *)message, 14);
  sei();

  for (;;) {
    timer1_wait();

    if (usart0_write_ready()) {
      usart0_write();
    }

    pin_toggle(LED_BUILTIN);
  }

  return 0;
}
