//
// System: platform
// Module: encoders
//
// Purpose:
// - Provides access to the encoder counts.
//
// Interrupts:
// - INT0 when the left encoder's A or B change
// - INT1 when the right encoder's A or B change
//
// Design:
// - Encoder counts are automatically accumulated in the delta variables.
// - The user must call encoders_update() frequently to include these deltas
//   in the full encoder counts.
//
#pragma once

#include <stdbool.h>
#include <stdint.h>

// Encoder counts.
extern int32_t encoders_left;
extern int32_t encoders_right;

// Changes to encoder counts since the last update.
extern int8_t encoders_left_delta;
extern int8_t encoders_right_delta;

// encoders_init initializes the encoders.
void encoders_init();

// encoders_update applies changes since the last update.
// NOTE: this must be called frequently so the deltas do not overflow!
void encoders_update();
