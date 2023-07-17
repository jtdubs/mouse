#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.h"
#include "firmware/platform/adc.h"
#include "firmware/platform/pin.h"
#include "sensor_cal_impl.h"
#include "walls_impl.h"

typedef struct {
  bool    left_present;
  bool    right_present;
  bool    forward_present;
  int16_t error_left;
  int16_t error_right;
} walls_state_t;

static bool          led_control;
static walls_state_t state;

void walls_init() {
  led_control = true;
}

void walls_led_control(bool enabled) {
  led_control = enabled;
}

void walls_update() {
  uint16_t left, right, forward;
  adc_read_sensors(&left, &right, &forward);

  uint16_t left_cal, right_cal, forward_cal;
  sensor_cal_read(&left_cal, &right_cal, &forward_cal);

  walls_state_t s;
  s.left_present    = (left >= (left_cal - 80));
  s.right_present   = (right >= (right_cal - 80));
  s.forward_present = (forward >= (forward_cal - 60));
  s.error_left      = left - left_cal;
  s.error_right     = right_cal - right;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    state = s;
  }

  if (led_control) {
    pin_set2(LED_LEFT, s.left_present);
    pin_set2(LED_RIGHT, s.right_present);
    pin_set2(LED_ONBOARD, s.forward_present);
  }
}

// walls_error returns the "centering error" of the mouse, based on wall distances.
float walls_error() {
  walls_state_t s;
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

// walls_present returns the presence of walls on each side of the mouse.
void walls_present(bool* left, bool* right, bool* forward) {
  assert(ASSERT_WALLS + 0, left != NULL);
  assert(ASSERT_WALLS + 1, right != NULL);
  assert(ASSERT_WALLS + 2, forward != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *left    = state.left_present;
    *right   = state.right_present;
    *forward = state.forward_present;
  }
}
