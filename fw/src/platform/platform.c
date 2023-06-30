#include "platform.h"

#include <avr/power.h>

#include "platform/adc.h"
#include "platform/encoders.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "platform/rtc.h"
#include "platform/timer.h"
#include "platform/usart0.h"

void platform_init() {
  // Turn off unused hardware to save a few mA.
  power_spi_disable();
  power_twi_disable();

  // Initialize all the platform modules.
  pin_init();
  usart0_init();
  adc_init();
  encoders_init();
  motor_init();
  timer_init();
  rtc_init();
}
