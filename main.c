#include <avr/io.h>
#include <avr/avr_mcu_section.h>
#include <util/delay.h>

AVR_MCU(F_CPU, "atmega328p");
AVR_MCU_VOLTAGES(5000, 5000, 5000);
AVR_MCU_VCD_PORT_PIN('B', 5, "PB5");

int main()
{
  DDRB |= 0x20; // Set PB5 as output

  for (;;)
  {
    PORTB ^= 0x20; // Toggle PB5
    _delay_ms(1);
  }

  return 0;
}
