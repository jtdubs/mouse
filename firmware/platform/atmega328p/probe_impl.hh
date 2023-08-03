#pragma once

#include <stdint.h>

#include "firmware/platform/probe.hh"
#include "pin_impl.hh"

namespace mouse::platform::probe {

namespace {

const pin::Pin kPins[] = {
    pin::kProbeTick,
    pin::kProbePlan,
    pin::kProbeUnused,
};

}

void Set(Probe probe, bool on) {
  pin::Set(kPins[static_cast<uint8_t>(probe)], on);
}

void Set(Probe probe) {
  pin::Set(kPins[static_cast<uint8_t>(probe)]);
}

void Clear(Probe probe) {
  pin::Clear(kPins[static_cast<uint8_t>(probe)]);
}

void Toggle(Probe probe) {
  pin::Toggle(kPins[static_cast<uint8_t>(probe)]);
}

bool IsSet(Probe probe) {
  return pin::IsSet(kPins[static_cast<uint8_t>(probe)]);
}

}  // namespace mouse::platform::probe
