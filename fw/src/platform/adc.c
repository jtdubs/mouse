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
  ADCSRA |= _BV(ADEN);                             // Enable ADC
}

// adc_sample samples the ADC channels.
void adc_sample() {
  // Start reading from channel 0.
  ADMUX   = (ADMUX & 0xF0);
  ADCSRA |= _BV(ADSC);
}

constexpr adc_channel_t next_channel[ADC_CHANNEL_COUNT] = {
    [ADC_SENSOR_RIGHT]    = ADC_SENSOR_CENTER,    //
    [ADC_SENSOR_CENTER]   = ADC_SENSOR_LEFT,      //
    [ADC_SENSOR_LEFT]     = ADC_SELECTOR,         //
    [ADC_SELECTOR]        = ADC_BATTERY_VOLTAGE,  //
    [ADC_BATTERY_VOLTAGE] = ADC_SENSOR_RIGHT,
};

ISR(ADC_vect, ISR_BLOCK) {
  adc_channel_t adc_index = ADMUX & 0x0F;

  // Store the ADC result.
  adc_values[adc_index] = ADC;

  // Select the next ADC channel
  ADMUX = (ADMUX & 0xF0) | next_channel[adc_index];

  if (adc_index != ADC_SENSOR_RIGHT) {
    // Start the next conversion
    ADCSRA |= _BV(ADSC);
  }
}
