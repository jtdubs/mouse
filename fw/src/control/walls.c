#include "walls.h"

#include <stddef.h>
#include <util/atomic.h>

#include "control/sensor_cal.h"
#include "platform/adc.h"
#include "platform/pin.h"
#include "utils/assert.h"

static bool wall_left_present;
static bool wall_right_present;
static bool wall_forward_present;

static int16_t wall_error_left;
static int16_t wall_error_right;

static bool wall_led_control;

void walls_init() {
  wall_led_control = true;
}

void walls_led_control(bool enabled) {
  wall_led_control = enabled;
}

void walls_update() {
  uint16_t left, front, right;
  adc_read_sensors(&left, &front, &right);

  uint16_t left_cal, right_cal, front_cal;
  sensor_cal_read(&left_cal, &right_cal, &front_cal);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    wall_left_present    = (left >= (left_cal - 80));
    wall_right_present   = (right >= (right_cal - 80));
    wall_forward_present = (front >= (front_cal - 60));
    wall_error_left      = left - left_cal;
    wall_error_right     = right_cal - right;
  }

  if (wall_led_control) {
    pin_set2(LED_LEFT, wall_left_present);
    pin_set2(LED_RIGHT, wall_right_present);
    pin_set2(LED_BUILTIN, wall_forward_present);
  }
}

// walls_error returns the "centering error" of the mouse, based on wall distances.
float walls_error() {
  float error = 0.0f;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (!wall_left_present && !wall_right_present) {
      error = 0;
    } else if (wall_left_present && !wall_right_present) {
      error = wall_error_left * 2.0f;
    } else if (!wall_left_present && wall_right_present) {
      error = wall_error_right * 2.0f;
    } else {
      error = (float)(wall_error_left + wall_error_right);
    }
  }
  return error;
}

// walls_present returns the presence of walls on each side of the mouse.
void walls_present(bool* left, bool* right, bool* forward) {
  assert(ASSERT_WALLS + 0, left != NULL);
  assert(ASSERT_WALLS + 1, right != NULL);
  assert(ASSERT_WALLS + 2, forward != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *left    = wall_left_present;
    *right   = wall_right_present;
    *forward = wall_forward_present;
  }
}
