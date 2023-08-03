//
// System: platform
// Module: led
//
// Purpose:
// - Management of LEDs.
//
#pragma once

#include <stdbool.h>
#include <stdint.h>

namespace mouse::platform::led {

enum class LED : uint8_t { Left, Right, Onboard, IR };

// Set sets the value of the led.
void Set(LED led, bool on);

// Set sets the led to high.
void Set(LED led);

// Clear sets the led to low.
void Clear(LED led);

// toggle toggles the led.
void Toggle(LED led);

// IsSet checks if the led is set.
bool IsSet(LED led);

}  // namespace mouse::platform::led
