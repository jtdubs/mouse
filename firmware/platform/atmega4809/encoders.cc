#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "encoders_impl.hh"
#include "firmware/lib/utils/assert.hh"
#include "pin_impl.hh"
#include "rtc_impl.hh"

namespace mouse::platform::encoders {

namespace {
// Encoder counts.
int32_t left_;
int32_t right_;

// Changes to encoder counts since the last update.
int8_t left_delta_;
int8_t right_delta_;
}  // namespace

// Init initializes the encoders.
void Init() {
  PORTA_PIN0CTRL = PORT_ISC_BOTHEDGES_gc;  // INT0
  PORTA_PIN1CTRL = PORT_ISC_BOTHEDGES_gc;  // INT1
  PORTA_DIRCLR   = _BV(0) | _BV(1);        // Set as input
}

// update applies changes since the last update.
void Update() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left_        += static_cast<int32_t>(left_delta_);
    right_       += static_cast<int32_t>(right_delta_);
    left_delta_   = 0;
    right_delta_  = 0;
  }
}

void Read(int32_t& left, int32_t& right) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left  = left_;
    right = right_;
  }
}

void ReadDeltas(int32_t& left, int32_t& right) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left  = left_delta_;
    right = right_delta_;
  }
}

// Left Encoder Clock
ISR(PORTA_PORT_vect, ISR_BLOCK) {
  if (PORTA_INTFLAGS & _BV(0)) {
    static uint8_t left_last_b = 0;

    // Read the encoder pins (b and clk) and calculate a.
    uint8_t d   = PIND;
    uint8_t b   = (d >> 4) & 1;
    uint8_t clk = ((d >> 2) & 1);
    uint8_t a   = clk ^ b;

    // Update the encoder count based on rotation direction.
    if (a == left_last_b) {
      left_delta_++;
    } else {
      left_delta_--;
    }

    left_last_b = b;
  }
  if (PORTA_INTFLAGS & _BV(1)) {
    static uint8_t right_last_b = 0;

    // Read the encoder pins (b and clk) and calculate a.
    uint8_t d   = PIND;
    uint8_t b   = (d >> 5) & 1;
    uint8_t clk = ((d >> 3) & 1);
    uint8_t a   = clk ^ b;

    // Update the encoder count based on rotation direction.
    if (a == right_last_b) {
      right_delta_--;
    } else {
      right_delta_++;
    }

    right_last_b = b;
  }
  PORTA_INTFLAGS = 3;
}

}  // namespace mouse::platform::encoders
