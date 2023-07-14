#include "sensor_cal.h"

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <util/atomic.h>

#include "control/config.h"
#include "platform/adc.h"
#include "platform/pin.h"
#include "utils/assert.h"
#include "utils/math.h"

constexpr uint16_t SensorCalSamplePower = 9;                            // dimensionless
constexpr uint16_t SensorCalSampleLimit = (1 << SensorCalSamplePower);  // dimensionless

static uint16_t sensor_threshold_left;     // ADC reading
static uint16_t sensor_threshold_right;    // ADC reading
static uint16_t sensor_threshold_forward;  // ADC reading

static uint32_t sensor_sum_left;
static uint32_t sensor_sum_right;
static uint32_t sensor_sum_forward;
static uint16_t sensor_sample_count;
static bool     sensor_leds_prev_state;

void sensor_cal_init() {
  // These are reasonable guesses in case calibration is not performed.
  sensor_threshold_left    = SENSOR_SIDE_DEFAULT_CAL;
  sensor_threshold_right   = SENSOR_SIDE_DEFAULT_CAL;
  sensor_threshold_forward = SENSOR_FRONT_DEFAULT_CAL;
}

void sensor_cal_start() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    sensor_sum_left     = 0;
    sensor_sum_right    = 0;
    sensor_sum_forward  = 0;
    sensor_sample_count = 0;
  }
  sensor_leds_prev_state = pin_is_set(IR_LEDS);
  pin_set(IR_LEDS);
}

bool sensor_cal_tick() {
  if (sensor_sample_count == SensorCalSampleLimit) {
    pin_set2(IR_LEDS, sensor_leds_prev_state);

    // The left and right thresholds are averaged to compensate for the mouse not being positioned
    // perfectly in the center of the corridor during calibration.
    uint16_t average = ((sensor_sum_left + sensor_sum_right) >> (SensorCalSamplePower + 1));

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      sensor_threshold_left    = average;
      sensor_threshold_right   = average;
      sensor_threshold_forward = sensor_sum_forward >> SensorCalSamplePower;
    }
    return true;
  }

  uint16_t left, right, forward;
  adc_read_sensors(&left, &right, &forward);
  sensor_sum_left    += left;
  sensor_sum_right   += right;
  sensor_sum_forward += forward;
  sensor_sample_count++;

  return false;
}

void sensor_cal_read(uint16_t *left, uint16_t *right, uint16_t *forward) {
  assert(ASSERT_SENSOR + 0, left != NULL);
  assert(ASSERT_SENSOR + 1, right != NULL);
  assert(ASSERT_SENSOR + 2, forward != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *left    = sensor_threshold_left;
    *right   = sensor_threshold_right;
    *forward = sensor_threshold_forward;
  }
}
