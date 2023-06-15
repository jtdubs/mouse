#include "pin.h"

#include <avr/avr_mcu_section.h>

// Set simavr configuration.
AVR_MCU(F_CPU, "atmega328p");
AVR_MCU_VOLTAGES(5000, 5000, 5000);
AVR_MCU_VCD_PORT_PIN('A', 0, "SERIAL_RX");
AVR_MCU_VCD_PORT_PIN('A', 1, "SERIAL_TX");
AVR_MCU_VCD_PORT_PIN('B', 5, "LED_BUILTIN");
AVR_MCU_VCD_PORT_PIN('C', 1, "SENSOR_0");
AVR_MCU_VCD_PORT_PIN('C', 1, "SENSOR_1");
AVR_MCU_VCD_PORT_PIN('C', 2, "SENSOR_2");
AVR_MCU_VCD_PORT_PIN('C', 3, "PROBE_0");
AVR_MCU_VCD_PORT_PIN('C', 4, "PROBE_1");
AVR_MCU_VCD_PORT_PIN('C', 5, "PROBE_2");
AVR_MCU_VCD_PORT_PIN('C', 6, "FSEL");
AVR_MCU_VCD_PORT_PIN('C', 7, "BATTERY");

// pin_init initializes all pins.
void pin_init() {
  pin_output(LED_BUILTIN);
  pin_output(PROBE_0);
  pin_output(PROBE_1);
  pin_output(PROBE_2);

  pin_clear(LED_BUILTIN);
  pin_clear(PROBE_0);
  pin_clear(PROBE_1);
  pin_clear(PROBE_2);
}
