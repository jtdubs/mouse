#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "adc_impl.hh"
#include "firmware/lib/utils/assert.hh"
#include "pin_impl.hh"

namespace adc {

namespace {
// Raw 10-bit readings from ADC channels.
uint16_t values[CHANNEL_COUNT];
}  // namespace

// init initializes the ADC.
void init() {
  ADMUX  = _BV(REFS0);                             // AVCC with external capacitor at AREF pin
  ADCSRA = _BV(ADIE)                               // Enable ADC interrupt
         | _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2);   // Prescaler 128 (slow but accurate ADC readings)
  ADCSRB  = 0;                                     // Free running mode
  DIDR0   = _BV(ADC0D) | _BV(ADC1D) | _BV(ADC2D);  // Disable digital input buffer on ADC2
  ADMUX  |= FIRST_CHANNEL;                         // Select the first channel.
  ADCSRA |= _BV(ADEN);                             // Enable ADC
}

// sample samples the ADC channels.
void sample() {
  ADCSRA |= _BV(ADSC);
}

uint16_t read(channel_t channel) {
  assert(assert::_ADC + 0, channel < CHANNEL_COUNT);

  uint16_t result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    result = values[channel];
  }
  return result;
}

void read_sensors(uint16_t& left, uint16_t& right, uint16_t& forward) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left    = values[SENSOR_LEFT];
    right   = values[SENSOR_RIGHT];
    forward = values[SENSOR_FORWARD];
  }
}

ISR(ADC_vect, ISR_BLOCK) {
  // Check which channel was just read, and which channel should be read next.
  channel_t index = (channel_t)(ADMUX & 0x0F);
  channel_t next  = NEXT_CHANNEL[index];

  // Store the ADC result.
  values[index] = ADC;

  // Select the next ADC channel
  ADMUX = (ADMUX & 0xF0) | next;

  // Start the next conversion, unless we are at the end of the sampling round.
  if (next != FIRST_CHANNEL) {
    ADCSRA |= _BV(ADSC);
  }
}

}  // namespace adc
