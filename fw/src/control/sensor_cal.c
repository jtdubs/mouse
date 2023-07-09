#include "sensor_cal.h"

#include <math.h>
#include <stdbool.h>
#include <util/atomic.h>

#include "control/config.h"
#include "platform/adc.h"
#include "platform/pin.h"
#include "utils/math.h"

constexpr uint16_t SensorCalSamplePower = 9;                            // dimensionless
constexpr uint16_t SensorCalSampleLimit = (1 << SensorCalSamplePower);  // dimensionless

uint16_t sensor_threshold_left;    // ADC reading
uint16_t sensor_threshold_right;   // ADC reading
uint16_t sensor_threshold_center;  // ADC reading

static uint32_t sensor_sum_left;
static uint32_t sensor_sum_right;
static uint32_t sensor_sum_center;
static uint16_t sensor_sample_count;

void sensor_cal_init() {
  sensor_threshold_left   = 30;
  sensor_threshold_right  = 30;
  sensor_threshold_center = 80;
}

void sensor_cal_start() {
  sensor_sum_left     = 0;
  sensor_sum_right    = 0;
  sensor_sum_center   = 0;
  sensor_sample_count = 0;

  pin_set(IR_LEDS);
}

bool sensor_cal_update() {
  if (sensor_sample_count == SensorCalSampleLimit) {
    pin_clear(IR_LEDS);
    sensor_threshold_left   = sensor_sum_left >> SensorCalSamplePower;
    sensor_threshold_right  = sensor_sum_right >> SensorCalSamplePower;
    sensor_threshold_center = sensor_sum_center >> SensorCalSamplePower;
    return true;
  }

  sensor_sum_left   += adc_values[ADC_SENSOR_LEFT];
  sensor_sum_right  += adc_values[ADC_SENSOR_RIGHT];
  sensor_sum_center += adc_values[ADC_SENSOR_CENTER];
  sensor_sample_count++;

  return false;
}
