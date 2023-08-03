//
// System: platform
// Module: probe
//
// Purpose:
// - Management of probes.
//
#pragma once

#include <stdbool.h>
#include <stdint.h>

namespace mouse::platform::probe {

enum class Probe : uint8_t { Tick, Plan, Unused };

// Set sets the value of the probe.
void Set(Probe probe, bool on);

// Set sets the probe to high.
void Set(Probe probe);

// Clear sets the probe to low.
void Clear(Probe probe);

// toggle toggles the probe.
void Toggle(Probe probe);

// IsSet checks if the probe is set.
bool IsSet(Probe probe);

}  // namespace mouse::platform::probe
