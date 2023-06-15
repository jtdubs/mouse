#include "adc.h"

#include <assert.h>
#include <avr/io.h>

#include "pin.h"

// adc_init initializes the ADC.
void adc_init() {
  ADMUX = (1 << REFS0)   // AVCC with external capacitor at AREF pin
        | (0 << REFS1)   // AVCC with external capacitor at AREF pin
        | (0 << ADLAR);  // Don't left adjust result

  ADCSRA = (0 << ADEN)    // Disable ADC
         | (0 << ADSC)    // Don't start conversion
         | (0 << ADATE)   // Disable auto trigger
         | (0 << ADIF)    // Clear interrupt flag
         | (0 << ADIE)    // Disable interrupt
         | (1 << ADPS0)   // Prescaler 128
         | (1 << ADPS1)   // Prescaler 128
         | (1 << ADPS2);  // Prescaler 128

  ADCSRB = (0 << ADTS0)   // Free running mode
         | (0 << ADTS1)   // Free running mode
         | (0 << ADTS2);  // Free running mode

  DIDR0 = (1 << ADC0D)   // Disable digital input buffer on ADC0
        | (1 << ADC1D)   // Disable digital input buffer on ADC1
        | (1 << ADC2D);  // Disable digital input buffer on ADC2

  ADCSRA |= (1 << ADEN);  // Enable ADC
}

// adc_read_0 reads the ADC0 value.
uint16_t adc_read(uint8_t n) {
  assert(!(ADCSRA & (1 << ADSC)));    // No conversion already running
  assert(n < 3 || n == 6 || n == 7);  // ADC channels 0,1,2,6,7 are available

  ADMUX   = (ADMUX & 0xF0) | (n & 0x0F);
  ADCSRA |= (1 << ADSC);        // Start conversion
  while (ADCSRA & (1 << ADSC))  // Wait for conversion
    ;
  uint16_t result  = ADCL;
  result          |= ADCH << 8;
  return result;
}
