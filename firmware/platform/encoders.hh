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

#include <stdint.h>

namespace encoders {

// update applies changes since the last update.
// NOTE: this must be called frequently so the deltas do not overflow!
void update();

// read reads the encoder counts.
void read(int32_t& left, int32_t& right);

// read_deltas reads the changes to the encoder counts since the last update.
void read_deltas(int32_t& left, int32_t& right);

}  // namespace encoders
