#include "encoders.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "platform/pin.h"
#include "platform/rtc.h"

// Encoder counts.
uint16_t encoder_left;
uint16_t encoder_right;

// Encoder timings.
uint32_t encoder_times_left[2];   // 0: current, 1: previous
uint32_t encoder_times_right[2];  // 0: current, 1: previous

// Encoder directions.
bool encoder_forward_left;
bool encoder_forward_right;

// encoders_init initializes the encoders.
void encoders_init() {
  EICRA = (1 << ISC00)   // Trigger INT0 on any logical change
        | (0 << ISC01)   // Trigger INT0 on any logical change
        | (1 << ISC10)   // Trigger INT1 on any logical change
        | (0 << ISC11);  // Trigger INT1 on any logical change
  EIMSK = (1 << INT0)    // Enable INT0
        | (1 << INT1);   // Enable INT1
}

// Left Encoder Clock
ISR(INT0_vect, ISR_BLOCK) {
  pin_toggle(PROBE_1);

  static uint8_t left_last_b = 0;

  // Read the encoder pins (b and clk) and calculate a.
  uint8_t d   = PIND;
  uint8_t b   = (d >> 4) & 1;
  uint8_t clk = ((d >> 2) & 1);
  uint8_t a   = clk ^ b;

  // Update the encoder count based on rotation direction.
  if (a == left_last_b) {
    encoder_left++;
    encoder_forward_left = true;
  } else {
    encoder_left--;
    encoder_forward_left = false;
  }

  encoder_times_left[1] = encoder_times_left[0];
  encoder_times_left[0] = rtc_micros();

  left_last_b = b;
}

// Right Encoder Clock
ISR(INT1_vect, ISR_BLOCK) {
  pin_toggle(PROBE_2);

  static uint8_t right_last_b = 0;

  // Read the encoder pins (b and clk) and calculate a.
  uint8_t d   = PIND;
  uint8_t b   = (d >> 5) & 1;
  uint8_t clk = ((d >> 3) & 1);
  uint8_t a   = clk ^ b;

  // Update the encoder count based on rotation direction.
  if (a == right_last_b) {
    encoder_right--;
    encoder_forward_right = false;
  } else {
    encoder_right++;
    encoder_forward_right = true;
  }

  encoder_times_right[1] = encoder_times_right[0];
  encoder_times_right[0] = rtc_micros();

  right_last_b = b;
}
