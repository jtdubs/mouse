#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "firmware/platform/adc.hh"
#include "firmware/platform/pin.hh"
#include "sensor_cal_impl.hh"
#include "walls_impl.hh"

namespace walls {

namespace {
bool    control_leds;
state_t state;
}  // namespace

void init() {
  control_leds = true;
}

void led_control(bool enabled) {
  control_leds = enabled;
}

void update() {
  uint16_t left, right, forward;
  adc::read_sensors(&left, &right, &forward);

  uint16_t left_cal, right_cal, forward_cal;
  sensor_cal::read(&left_cal, &right_cal, &forward_cal);

  state_t s;
  s.left_present    = (left >= (left_cal - 80));
  s.right_present   = (right >= (right_cal - 80));
  s.forward_present = (forward >= (forward_cal - 60));
  s.error_left      = left - left_cal;
  s.error_right     = right_cal - right;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state = s;
  }

  if (control_leds) {
    pin::set2(pin::LED_LEFT, s.left_present);
    pin::set2(pin::LED_RIGHT, s.right_present);
    pin::set2(pin::LED_ONBOARD, s.forward_present);
  }
}

// error returns the "centering error" of the mouse, based on wall distances.
float error() {
  state_t s;
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
void present(bool* left, bool* right, bool* forward) {
  assert(assert::WALLS + 0, left != NULL);
  assert(assert::WALLS + 1, right != NULL);
  assert(assert::WALLS + 2, forward != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *left    = state.left_present;
    *right   = state.right_present;
    *forward = state.forward_present;
  }
}

}  // namespace walls
