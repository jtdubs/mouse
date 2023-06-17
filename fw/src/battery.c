#include "battery.h"

#include "adc.h"

uint8_t battery_voltage = 0;

void battery_update() {
  battery_voltage = adc_read(7) >> 2;
}
