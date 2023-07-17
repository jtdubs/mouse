#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <util/atomic.h>

#include "config.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/math.hh"
#include "firmware/platform/adc.hh"
#include "firmware/platform/pin.hh"
#include "sensor_cal_impl.hh"

namespace sensor_cal {

namespace {
constexpr uint16_t SamplePower = 9;                   // dimensionless
constexpr uint16_t SampleLimit = (1 << SamplePower);  // dimensionless

uint16_t threshold_left;     // ADC reading
uint16_t threshold_right;    // ADC reading
uint16_t threshold_forward;  // ADC reading

uint32_t sum_left;
uint32_t sum_right;
uint32_t sum_forward;
uint16_t sample_count;
bool     leds_prev_state;
}  // namespace

void init() {
  // These are reasonable guesses in case calibration is not performed.
  threshold_left    = SENSOR_SIDE_DEFAULT_CAL;
  threshold_right   = SENSOR_SIDE_DEFAULT_CAL;
  threshold_forward = SENSOR_FRONT_DEFAULT_CAL;
}

void start() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    sum_left     = 0;
    sum_right    = 0;
    sum_forward  = 0;
    sample_count = 0;
  }
  leds_prev_state = pin::is_set(pin::IR_LEDS);
  pin::set(pin::IR_LEDS);
}

bool tick() {
  if (sample_count == SampleLimit) {
    pin::set2(pin::IR_LEDS, leds_prev_state);

    // The left and right thresholds are averaged to compensate for the mouse not being positioned
    // perfectly in the center of the corridor during calibration.
    uint16_t average = ((sum_left + sum_right) >> (SamplePower + 1));

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      threshold_left    = average;
      threshold_right   = average;
      threshold_forward = sum_forward >> SamplePower;
    }
    return true;
  }

  uint16_t left, right, forward;
  adc::read_sensors(&left, &right, &forward);
  sum_left    += left;
  sum_right   += right;
  sum_forward += forward;
  sample_count++;

  return false;
}

void read(uint16_t *left, uint16_t *right, uint16_t *forward) {
  assert(assert::SENSOR + 0, left != NULL);
  assert(assert::SENSOR + 1, right != NULL);
  assert(assert::SENSOR + 2, forward != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *left    = threshold_left;
    *right   = threshold_right;
    *forward = threshold_forward;
  }
}

}  // namespace sensor_cal
