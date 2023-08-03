#include <avr/power.h>
#include <stddef.h>
#include <util/atomic.h>

#include "adc_impl.hh"
#include "encoders_impl.hh"
#include "firmware/lib/utils/assert.hh"
#include "led_impl.hh"
#include "motor_impl.hh"
#include "pin_impl.hh"
#include "platform_impl.hh"
#include "probe_impl.hh"
#include "rtc_impl.hh"
#include "timer_impl.hh"
#include "usart0_impl.hh"

namespace mouse::platform {

void Init() {
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
  assert(assert::Module::Platform, 0, buffer != NULL);
  assert(assert::Module::Platform, 1, len >= sizeof(Report));

  uint16_t left, right, forward;
  adc::ReadSensors(left, right, forward);

  Report *report = (Report *)buffer;
  encoders::Read(report->encoders.left, report->encoders.right);

  motor::Read(report->motors.left, report->motors.right);

  report->leds.left       = led::IsSet(led::LED::Left);
  report->leds.right      = led::IsSet(led::LED::Right);
  report->leds.onboard    = led::IsSet(led::LED::Onboard);
  report->leds.ir         = led::IsSet(led::LED::IR);
  report->sensors.left    = left;
  report->sensors.right   = right;
  report->sensors.forward = forward;

  return sizeof(Report);
}

}  // namespace mouse::platform
