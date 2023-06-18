#include "adc.h"

#include <assert.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "pin.h"

uint16_t       adc_values[8] = {0};
static uint8_t adc_index     = 0;

// adc_init initializes the ADC.
void adc_init() {
  ADMUX = (1 << REFS0)   // AVCC with external capacitor at AREF pin
        | (0 << REFS1)   // AVCC with external capacitor at AREF pin
        | (1 << ADLAR);  // Left adjust result

  ADCSRA = (0 << ADEN)    // Disable ADC
         | (0 << ADSC)    // Don't start conversion
         | (0 << ADATE)   // Disable auto trigger
         | (0 << ADIF)    // Clear interrupt flag
         | (1 << ADIE)    // Enable interrupt
         | (0 << ADPS0)   // Prescaler 64
         | (1 << ADPS1)   // Prescaler 64
         | (1 << ADPS2);  // Prescaler 64

  ADCSRB = (0 << ADTS0)   // Free running mode
         | (0 << ADTS1)   // Free running mode
         | (0 << ADTS2);  // Free running mode

  DIDR0 = (1 << ADC0D)   // Disable digital input buffer on ADC0
        | (1 << ADC1D)   // Disable digital input buffer on ADC1
        | (1 << ADC2D);  // Disable digital input buffer on ADC2

  ADCSRA |= (1 << ADEN);  // Enable ADC
  ADCSRA |= (1 << ADSC);  // Start the first conversion
}

ISR(ADC_vect, ISR_BLOCK) {
  adc_values[adc_index] = ADC;

  // Advance to next ADC index.
  if (adc_index == 2) {
    adc_index = 6;
  } else {
    adc_index = (adc_index + 1) & 7;
  }

  // Start the next conversion
  ADMUX   = (ADMUX & 0xF0) | adc_index;
  ADCSRA |= (1 << ADSC);
}
