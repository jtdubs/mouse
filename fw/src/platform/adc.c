#include "platform/adc.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "platform/pin.h"

// Raw 10-bit readings from ADC channels.
uint16_t adc_values[ADC_CHANNEL_COUNT];

// adc_init initializes the ADC.
void adc_init() {
  ADMUX  = _BV(REFS0);                             // AVCC with external capacitor at AREF pin
  ADCSRA = _BV(ADIE)                               // Enable ADC interrupt
         | _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2);   // Prescaler 128 (slow but accurate ADC readings)
  ADCSRB  = 0;                                     // Free running mode
  DIDR0   = _BV(ADC0D) | _BV(ADC1D) | _BV(ADC2D);  // Disable digital input buffer on ADC2
  ADMUX  |= ADC_FIRST_CHANNEL;                     // Select the first channel.
  ADCSRA |= _BV(ADEN);                             // Enable ADC
}

// adc_sample samples the ADC channels.
void adc_sample() {
  ADCSRA |= _BV(ADSC);
}

ISR(ADC_vect, ISR_BLOCK) {
  // Check which channel was just read, and which channel should be read next.
  adc_channel_t adc_index = ADMUX & 0x0F;
  adc_channel_t adc_next  = ADC_NEXT_CHANNEL[adc_index];

  // Store the ADC result.
  adc_values[adc_index] = ADC;

  // Select the next ADC channel
  ADMUX = (ADMUX & 0xF0) | adc_next;

  // Start the next conversion, unless we are at the end of the sampling round.
  if (adc_next != ADC_FIRST_CHANNEL) {
    ADCSRA |= _BV(ADSC);
  }
}
