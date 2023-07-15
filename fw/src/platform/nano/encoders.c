#if defined(BOARD_NANO)

#include "encoders.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "pin.h"
#include "rtc.h"
#include "utils/assert.h"

// Encoder counts.
static int32_t encoders_left;
static int32_t encoders_right;

// Changes to encoder counts since the last update.
static int8_t encoders_left_delta;
static int8_t encoders_right_delta;

// encoders_init initializes the encoders.
void encoders_init() {
  EICRA = _BV(ISC00)   // Trigger INT0 on any logical change
        | _BV(ISC10);  // Trigger INT1 on any logical change
  EIMSK = _BV(INT0)    // Enable INT0
        | _BV(INT1);   // Enable INT1
}

// encoders_update applies changes since the last update.
void encoders_update() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    encoders_left        += (int32_t)encoders_left_delta;
    encoders_right       += (int32_t)encoders_right_delta;
    encoders_left_delta   = 0;
    encoders_right_delta  = 0;
  }
}

void encoders_read(int32_t* left, int32_t* right) {
  assert(ASSERT_ENCODERS + 0, left != NULL);
  assert(ASSERT_ENCODERS + 1, right != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *left  = encoders_left;
    *right = encoders_right;
  }
}

void encoders_read_deltas(int32_t* left, int32_t* right) {
  assert(ASSERT_ENCODERS + 2, left != NULL);
  assert(ASSERT_ENCODERS + 3, right != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *left  = encoders_left_delta;
    *right = encoders_right_delta;
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
    encoders_left_delta++;
  } else {
    encoders_left_delta--;
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
    encoders_right_delta--;
  } else {
    encoders_right_delta++;
  }

  right_last_b = b;
}

#endif
