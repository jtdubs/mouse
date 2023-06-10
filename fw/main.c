#include <avr/avr_mcu_section.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "usart0.h"

AVR_MCU(F_CPU, "atmega328p");
AVR_MCU_VOLTAGES(5000, 5000, 5000);

int main() {
  init_usart0();
  sei();

  DDRB |= 0xFF;  // Set PB4-7 as output
  for (;;) {
    PORTB ^= 0x40;
    _delay_ms(1);
  }

  return 0;
}
