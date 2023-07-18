#include <avr/power.h>
#include <stddef.h>
#include <util/atomic.h>

#include "adc_impl.hh"
#include "encoders_impl.hh"
#include "firmware/lib/utils/assert.hh"
#include "motor_impl.hh"
#include "pin_impl.hh"
#include "platform_impl.hh"
#include "rtc_impl.hh"
#include "timer_impl.hh"
#include "usart0_impl.hh"

namespace platform {

void Init() {
  // Turn off unused hardware to save a few mA.
  power_spi_disable();
  power_twi_disable();

  // Initialize all the platform modules.
  pin::Init();
  usart0::Init();
  adc::Init();
  encoders::Init();
  motor::Init();
  timer::Init();
  rtc::Init();
}

uint8_t GetReport(uint8_t *buffer, [[maybe_unused]] uint8_t len) {
  assert(assert::PLATFORM + 0, buffer != NULL);
  assert(assert::PLATFORM + 1, len >= sizeof(Report));

  uint16_t left, right, forward;
  adc::read_sensors(left, right, forward);

  Report *report = (Report *)buffer;
  encoders::Read(report->encoders.left, report->encoders.right);

  motor::Read(report->motors.left, report->motors.right);

  report->leds.left       = pin::is_set(pin::kLEDLeft);
  report->leds.right      = pin::is_set(pin::kLEDRight);
  report->leds.onboard    = pin::is_set(pin::kLEDOnboard);
  report->leds.ir         = pin::is_set(pin::IR_LEDS);
  report->sensors.left    = left;
  report->sensors.right   = right;
  report->sensors.forward = forward;

  return sizeof(Report);
}

}  // namespace platform
