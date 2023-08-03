#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <util/atomic.h>

#include "firmware/config.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/math.hh"
#include "firmware/platform/platform.hh"
#include "sensor_cal_impl.hh"

namespace mouse::control::sensor_cal {

namespace {
constexpr uint16_t kSamplePower = 9;                    // dimensionless
constexpr uint16_t kSampleLimit = (1 << kSamplePower);  // dimensionless

uint16_t threshold_left_;     // ADC reading
uint16_t threshold_right_;    // ADC reading
uint16_t threshold_forward_;  // ADC reading

uint32_t sum_left_;
uint32_t sum_right_;
uint32_t sum_forward_;
uint16_t sample_count_;
bool     leds_prev_state_;
}  // namespace

void Init() {
  // These are reasonable guesses in case calibration is not performed.
  threshold_left_    = config::kSensorSideDefaultCal;
  threshold_right_   = config::kSensorSideDefaultCal;
  threshold_forward_ = config::kSensorFrontDefaultCal;
}

void Start() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    sum_left_     = 0;
    sum_right_    = 0;
    sum_forward_  = 0;
    sample_count_ = 0;
  }
  leds_prev_state_ = platform::led::IsSet(platform::led::LED::IR);
  platform::led::Set(platform::led::LED::IR);
}

bool Tick() {
  if (sample_count_ == kSampleLimit) {
    platform::led::Set(platform::led::LED::IR, leds_prev_state_);

    // The left and right thresholds are averaged to compensate for the mouse not being positioned
    // perfectly in the center of the corridor during calibration.
    uint16_t average = ((sum_left_ + sum_right_) >> (kSamplePower + 1));

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      threshold_left_    = average;
      threshold_right_   = average;
      threshold_forward_ = sum_forward_ >> kSamplePower;
    }
    return true;
  }

  uint16_t left, right, forward;
  platform::adc::ReadSensors(left, right, forward);
  sum_left_    += left;
  sum_right_   += right;
  sum_forward_ += forward;
  sample_count_++;

  return false;
}

void Read(uint16_t &left, uint16_t &right, uint16_t &forward) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left    = threshold_left_;
    right   = threshold_right_;
    forward = threshold_forward_;
  }
}

}  // namespace mouse::control::sensor_cal
