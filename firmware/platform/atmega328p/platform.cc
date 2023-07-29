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

namespace mouse::platform {

void Init() {
  // Turn off unused hardware to save a few mA.
  power_spi_disable();
  power_twi_disable();

  // Initialize all the platform modules.
  platform::pin::Init();
  platform::usart0::Init();
  platform::adc::Init();
  platform::encoders::Init();
  platform::motor::Init();
  platform::timer::Init();
  platform::rtc::Init();
}

uint8_t GetReport(uint8_t *buffer, [[maybe_unused]] uint8_t len) {
  assert(assert::Module::Platform, 0, buffer != NULL);
  assert(assert::Module::Platform, 1, len >= sizeof(Report));

  uint16_t left, right, forward;
  platform::adc::ReadSensors(left, right, forward);

  Report *report = (Report *)buffer;
  platform::encoders::Read(report->encoders.left, report->encoders.right);

  platform::motor::Read(report->motors.left, report->motors.right);

  report->leds.left       = platform::pin::IsSet(platform::pin::kLEDLeft);
  report->leds.right      = platform::pin::IsSet(platform::pin::kLEDRight);
  report->leds.onboard    = platform::pin::IsSet(platform::pin::kLEDOnboard);
  report->leds.ir         = platform::pin::IsSet(platform::pin::kIRLEDs);
  report->sensors.left    = left;
  report->sensors.right   = right;
  report->sensors.forward = forward;

  return sizeof(Report);
}

}  // namespace mouse::platform
