#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "firmware/platform/platform.hh"
#include "sensor_cal_impl.hh"
#include "walls_impl.hh"

namespace walls {

namespace {
bool  control_leds;
State state;
}  // namespace

void Init() {
  control_leds = true;
}

void ControlLEDs(bool enabled) {
  control_leds = enabled;
}

void Update() {
  uint16_t left, right, forward;
  adc::read_sensors(left, right, forward);

  uint16_t left_cal, right_cal, forward_cal;
  sensor_cal::Read(left_cal, right_cal, forward_cal);

  State s;
  s.left_present    = (left >= (left_cal - 80));
  s.right_present   = (right >= (right_cal - 80));
  s.forward_present = (forward >= (forward_cal - 60));
  s.error_left      = left - left_cal;
  s.error_right     = right_cal - right;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state = s;
  }

  if (control_leds) {
    pin::Set2(pin::kLEDLeft, s.left_present);
    pin::Set2(pin::kLEDRight, s.right_present);
    pin::Set2(pin::kLEDOnboard, s.forward_present);
  }
}

// error returns the "centering error" of the mouse, based on wall distances.
float error() {
  State s;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    s = state;
  }

  if (!s.left_present && !s.right_present) {
    return 0;
  } else if (s.left_present && !s.right_present) {
    return s.error_left * 2.0f;
  } else if (!s.left_present && s.right_present) {
    return s.error_right * 2.0f;
  } else {
    return (float)(s.error_left + s.error_right);
  }
}

// present returns the presence of walls on each side of the mouse.
void Present(bool& left, bool& right, bool& forward) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left    = state.left_present;
    right   = state.right_present;
    forward = state.forward_present;
  }
}

}  // namespace walls
