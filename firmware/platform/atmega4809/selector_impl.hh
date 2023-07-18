#pragma once

#include "firmware/platform/selector.hh"

namespace selector {

// Voltage thresholds for the DIP switches.
constexpr uint8_t SelectorThresholds[16] = {  //
    21, 42, 60, 77, 91, 102, 112, 123, 133, 139, 144, 150, 156, 160, 163, 255};

}  // namespace selector
