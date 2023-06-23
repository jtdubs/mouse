#include "platform/adc.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "platform/pin.h"

// Raw 10-bit readings from ADC channels.
uint16_t adc_sensor_right;
uint16_t adc_sensor_center;
uint16_t adc_sensor_left;
uint16_t adc_selector;
uint16_t adc_battery_voltage;

// adc_init initializes the ADC.
void adc_init() {
  ADMUX = (1 << REFS0)   // AVCC with external capacitor at AREF pin
        | (0 << REFS1)   // AVCC with external capacitor at AREF pin
        | (0 << ADLAR);  // Don't left adjust result

  ADCSRA = (0 << ADEN)    // Disable ADC
         | (0 << ADSC)    // Don't start conversion
         | (0 << ADATE)   // Disable auto trigger
         | (0 << ADIF)    // Clear interrupt flag
         | (1 << ADIE)    // Enable interrupt
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
  ADCSRA |= (1 << ADSC);  // Start the first conversion
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

  // Start the next conversion.
  ADMUX   = (ADMUX & 0xF0) | adc_index;
  ADCSRA |= (1 << ADSC);
}
