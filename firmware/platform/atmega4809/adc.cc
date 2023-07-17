#include "adc.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.h"
#include "pin.h"

// adc_next_channel defines the sequence in which channels are read.
constexpr adc_channel_t ADC_NEXT_CHANNEL[ADC_CHANNEL_COUNT] = {
    [ADC_SENSOR_RIGHT]    = ADC_SENSOR_FORWARD,   //
    [ADC_SENSOR_FORWARD]  = ADC_SENSOR_LEFT,      //
    [ADC_SENSOR_LEFT]     = ADC_SELECTOR,         //
    [ADC_SELECTOR]        = ADC_BATTERY_VOLTAGE,  //
    [ADC_BATTERY_VOLTAGE] = ADC_SENSOR_RIGHT,
};

// adc_first_channel is the first channel to be read.
constexpr adc_channel_t ADC_FIRST_CHANNEL = ADC_SENSOR_RIGHT;

// Raw 10-bit readings from ADC channels.
static uint16_t adc_values[ADC_CHANNEL_COUNT];

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

void adc_read(adc_channel_t channel, uint16_t* value) {
  assert(ASSERT_ADC + 0, channel < ADC_CHANNEL_COUNT);
  assert(ASSERT_ADC + 1, value != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *value = adc_values[channel];
  }
}

void adc_read_sensors(uint16_t* left, uint16_t* right, uint16_t* forward) {
  assert(ASSERT_ADC + 2, left != NULL);
  assert(ASSERT_ADC + 3, right != NULL);
  assert(ASSERT_ADC + 4, forward != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *left    = adc_values[ADC_SENSOR_LEFT];
    *right   = adc_values[ADC_SENSOR_RIGHT];
    *forward = adc_values[ADC_SENSOR_FORWARD];
  }
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
