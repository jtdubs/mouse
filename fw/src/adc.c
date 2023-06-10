#include "adc.h"

#include <avr/io.h>

void adc_init() {
  ADMUX  = (1 << REFS0) | (1 << ADLAR);
  ADCSRB = 0;  // free running mode
  ADCSRA = (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2) | (1 << ADATE) | (1 << ADSC);
}

uint16_t adc_read_0() {
  return ADCH;
}
