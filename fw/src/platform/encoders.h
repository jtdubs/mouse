#pragma once

#include <stdbool.h>
#include <stdint.h>

// Encoder counts.
extern uint16_t encoder_left;
extern uint16_t encoder_right;

// Encoder timings.
extern uint32_t encoder_times_left[2];   // 0: current, 1: previous
extern uint32_t encoder_times_right[2];  // 0: current, 1: previous

// Encoder directions.
extern bool encoder_forward_left;
extern bool encoder_forward_right;

// encoders_init initializes the encoders.
void encoders_init();
