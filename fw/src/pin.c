#include "pin.h"

#include <avr/avr_mcu_section.h>

AVR_MCU(F_CPU, "atmega328p");
AVR_MCU_VOLTAGES(5000, 5000, 5000);
AVR_MCU_VCD_PORT_PIN('B', 5, "LED_BUILTIN");
AVR_MCU_VCD_PORT_PIN('C', 3, "PROBE_0");
AVR_MCU_VCD_PORT_PIN('C', 4, "PROBE_1");
AVR_MCU_VCD_PORT_PIN('C', 5, "PROBE_2");

void pin_init() {
  DDRB |= _BV(5);
  DDRC |= _BV(3) | _BV(4) | _BV(5);
}
