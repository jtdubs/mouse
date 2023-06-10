#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "pin.h"
#include "usart0.h"

int main() {
  pin_init();
  usart0_init();
  sei();

  for (;;) {
    pin_toggle(LED_BUILTIN);
    _delay_ms(500);
  }

  return 0;
}
