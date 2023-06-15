#include "battery.h"

#include "adc.h"

uint16_t battery_voltage;

void battery_init() {
  battery_voltage = 0;
}

void battery_update() {
  uint16_t temp = adc_read(7);
  // Approximate voltage as 9.75*ADC, but true ratio is 9.765625.
  battery_voltage = (temp << 3) + (temp << 1) - (temp >> 2);
}
