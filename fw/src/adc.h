#pragma once

#include <assert.h>
#include <stdint.h>

extern uint16_t adc_values[8];

void adc_init();

inline uint16_t adc_read(uint8_t n) {
  assert(n < 3 || n == 6 || n == 7);  // ADC channels 0,1,2,6,7 are available
  return adc_values[n];
}
