#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "adc_impl.hh"
#include "firmware/lib/utils/assert.hh"
#include "pin_impl.hh"

namespace mouse::platform::adc {

namespace {
// Raw 10-bit readings from ADC channels.
uint16_t values[8];
}  // namespace

// Init initializes the ADC.
void Init() {
  ADC0_CTRLA = ADC_RESSEL_10BIT_gc;  // 10 bit readings
  ADC0_CTRLB = 0;                    // no accumulation
  ADC0_CTRLC = ADC_SAMPCAP_bm        // reduced sampling cap
             | ADC_REFSEL_VDDREF_gc  // VDD reference
             | ADC_PRESC_DIV128_gc;  // Prescaler 128
  ADC0_CTRLD = ADC_INITDLY_DLY16_gc  // Delay 16 before first sample
             | ADC_ASDV_ASVOFF_gc    // No auto sample variation
             | 3;                    // Delay 3 between samples
  ADC0_CTRLE     = 0;                // No window comparator
  ADC0_SAMPCTRL  = 0;                // No extra sampling time
  ADC0_MUXPOS    = kFirstChannel;    // Select first channel
  ADC0_INTCTRL   = ADC_RESRDY_bm;    // Enable result ready interrupt
  ADC0_CTRLA    |= ADC_ENABLE_bm;    // Enable ADC
}

// sample samples the ADC channels.
void Sample() {
  ADC0_COMMAND = ADC_STCONV_bm;  // Start conversion
}

uint16_t Read(Channel channel) {
  uint16_t result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    result = values[channel];
  }
  return result;
}

void ReadSensors(uint16_t& left, uint16_t& right, uint16_t& forward) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left    = values[Channel::SensorLeft];
    right   = values[Channel::SensorRight];
    forward = values[Channel::SensorForward];
  }
}

ISR(ADC0_RESRDY_vect, ISR_BLOCK) {
  // Check which channel was just read, and which channel should be read next.
  size_t index = ADC0_MUXPOS;

  // Store the ADC result.
  values[index] = ADC0_RES;

  // Select the next ADC channel
  ADC0_MUXPOS = kNextChannel[index];

  // Start the next conversion, unless we are at the end of the sampling round.
  if (next != kFirstChannel) {
    ADC0_COMMAND = ADC_STCONV_bm;  // Start conversion
  }
}

}  // namespace mouse::platform::adc
