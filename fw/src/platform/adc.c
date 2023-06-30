#include "platform/adc.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "platform/pin.h"

// Raw 10-bit readings from the ADC channels.
uint16_t adc_sensor_right;
uint16_t adc_sensor_center;
uint16_t adc_sensor_left;
uint16_t adc_selector;
uint16_t adc_battery_voltage;

// adc_init initializes the ADC.
void adc_init() {
  ADMUX  = _BV(REFS0);                             // AVCC with external capacitor at AREF pin
  ADCSRA = _BV(ADIE)                               // Enable ADC interrupt
         | _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2);   // Prescaler 128 (slow but accurate ADC readings)
  ADCSRB  = 0;                                     // Free running mode
  DIDR0   = _BV(ADC0D) | _BV(ADC1D) | _BV(ADC2D);  // Disable digital input buffer on ADC2
  ADCSRA |= _BV(ADEN);                             // Enable ADC
}

// adc_sample samples the ADC channels.
void adc_sample() {
  // Start reading from channel 0.
  ADMUX   = (ADMUX & 0xF0);
  ADCSRA |= _BV(ADSC);
}

ISR(ADC_vect, ISR_BLOCK) {
  uint8_t adc_index = ADMUX & 0x0F;

  // Choose the next ADC channel to sample.
  switch (adc_index) {
    case 0:
      adc_sensor_right = ADC;
      adc_index        = 1;
      break;
    case 1:
      adc_sensor_center = ADC;
      adc_index         = 2;
      break;
    case 2:
      adc_sensor_left = ADC;
      adc_index       = 6;
      break;
    case 6:
      adc_selector = ADC;
      adc_index    = 7;
      break;
    case 7:
      adc_battery_voltage = ADC;
      adc_index           = 0;
      break;
  }

  // Select the next ADC channel
  ADMUX = (ADMUX & 0xF0) | adc_index;

  if (adc_index != 0) {
    // Start the next conversion
    ADCSRA |= _BV(ADSC);
  }
}
