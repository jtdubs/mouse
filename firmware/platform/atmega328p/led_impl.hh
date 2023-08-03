#pragma once

#include <stdint.h>

#include "firmware/platform/led.hh"
#include "pin_impl.hh"

namespace mouse::platform::led {

namespace {

const pin::Pin kPins[] = {
    pin::kLEDLeft,
    pin::kLEDRight,
    pin::kLEDOnboard,
    pin::kIRLEDs,
};

}

void Set(LED led, bool on) {
  pin::Set(kPins[static_cast<uint8_t>(led)], on);
}

void Set(LED led) {
  pin::Set(kPins[static_cast<uint8_t>(led)]);
}

void Clear(LED led) {
  pin::Clear(kPins[static_cast<uint8_t>(led)]);
}

void Toggle(LED led) {
  pin::Toggle(kPins[static_cast<uint8_t>(led)]);
}

bool IsSet(LED led) {
  return pin::IsSet(kPins[static_cast<uint8_t>(led)]);
}

}  // namespace mouse::platform::led
