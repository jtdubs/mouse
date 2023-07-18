#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "encoders_impl.hh"
#include "firmware/lib/utils/assert.hh"
#include "pin_impl.hh"
#include "rtc_impl.hh"

namespace encoders {

namespace {
// Encoder counts.
int32_t left;
int32_t right;

// Changes to encoder counts since the last update.
int8_t left_delta;
int8_t right_delta;
}  // namespace

// Init initializes the encoders.
void Init() {
  EICRA = _BV(ISC00)   // Trigger INT0 on any logical change
        | _BV(ISC10);  // Trigger INT1 on any logical change
  EIMSK = _BV(INT0)    // Enable INT0
        | _BV(INT1);   // Enable INT1
}

// update applies changes since the last update.
void Update() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left        += (int32_t)left_delta;
    right       += (int32_t)right_delta;
    left_delta   = 0;
    right_delta  = 0;
  }
}

void Read(int32_t& left, int32_t& right) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left  = encoders::left;
    right = encoders::right;
  }
}

void ReadDeltas(int32_t& left, int32_t& right) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left  = left_delta;
    right = right_delta;
  }
}

// Left Encoder Clock
ISR(INT0_vect, ISR_BLOCK) {
  static uint8_t left_last_b = 0;

  // Read the encoder pins (b and clk) and calculate a.
  uint8_t d   = PIND;
  uint8_t b   = (d >> 4) & 1;
  uint8_t clk = ((d >> 2) & 1);
  uint8_t a   = clk ^ b;

  // Update the encoder count based on rotation direction.
  if (a == left_last_b) {
    left_delta++;
  } else {
    left_delta--;
  }

  left_last_b = b;
}

// Right Encoder Clock
ISR(INT1_vect, ISR_BLOCK) {
  static uint8_t right_last_b = 0;

  // Read the encoder pins (b and clk) and calculate a.
  uint8_t d   = PIND;
  uint8_t b   = (d >> 5) & 1;
  uint8_t clk = ((d >> 3) & 1);
  uint8_t a   = clk ^ b;

  // Update the encoder count based on rotation direction.
  if (a == right_last_b) {
    right_delta--;
  } else {
    right_delta++;
  }

  right_last_b = b;
}

}  // namespace encoders
