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

void init() {
  // Turn off unused hardware to save a few mA.
  power_spi_disable();
  power_twi_disable();

  // Initialize all the platform modules.
  pin::init();
  usart0::init();
  adc::init();
  encoders::init();
  motor::init();
  timer::init();
  rtc::init();
}

uint8_t report(uint8_t *buffer, [[maybe_unused]] uint8_t len) {
  assert(assert::PLATFORM + 0, buffer != NULL);
  assert(assert::PLATFORM + 1, len >= sizeof(report_t));

  uint16_t left, right, forward;
  adc::read_sensors(left, right, forward);

  report_t *report = (report_t *)buffer;
  encoders::read(report->encoders.left, report->encoders.right);

  motor::read(report->motors.left, report->motors.right);

  report->leds.left       = pin::is_set(pin::LED_LEFT);
  report->leds.right      = pin::is_set(pin::LED_RIGHT);
  report->leds.onboard    = pin::is_set(pin::LED_ONBOARD);
  report->leds.ir         = pin::is_set(pin::IR_LEDS);
  report->sensors.left    = left;
  report->sensors.right   = right;
  report->sensors.forward = forward;

  return sizeof(report_t);
}

}  // namespace platform
