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
uint16_t values[8];
}  // namespace

// Init initializes the ADC.
void Init() {
  ADMUX  = _BV(REFS0);                             // AVCC with external capacitor at AREF pin
  ADCSRA = _BV(ADIE)                               // Enable ADC interrupt
         | _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2);   // Prescaler 128 (slow but accurate ADC readings)
  ADCSRB  = 0;                                     // Free running mode
  DIDR0   = _BV(ADC0D) | _BV(ADC1D) | _BV(ADC2D);  // Disable digital input buffer on ADC2
  ADMUX  |= (uint8_t)kFirstChannel;                // Select the first channel.
  ADCSRA |= _BV(ADEN);                             // Enable ADC
}

// sample samples the ADC channels.
void Sample() {
  ADCSRA |= _BV(ADSC);
}

uint16_t Read(Channel channel) {
  uint16_t result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    result = values[(uint8_t)channel];
  }
  return result;
}

void ReadSensors(uint16_t& left, uint16_t& right, uint16_t& forward) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left    = values[(uint8_t)Channel::SensorLeft];
    right   = values[(uint8_t)Channel::SensorRight];
    forward = values[(uint8_t)Channel::SensorForward];
  }
}

ISR(ADC_vect, ISR_BLOCK) {
  // Check which channel was just read, and which channel should be read next.
  size_t  index = (ADMUX & 0x0F);
  Channel next  = kNextChannel[index];

  // Store the ADC result.
  values[index] = ADC;

  // Select the next ADC channel
  ADMUX = (ADMUX & 0xF0) | (uint8_t)next;

  // Start the next conversion, unless we are at the end of the sampling round.
  if (next != kFirstChannel) {
    ADCSRA |= _BV(ADSC);
  }
}

}  // namespace adc
