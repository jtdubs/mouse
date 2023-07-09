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

bool platform_report(platform_report_t *report) {
  report->encoders.left  = encoders_left;
  report->encoders.right = encoders_right;
  report->leds.ir        = pin_is_set(IR_LEDS);
  report->leds.left      = pin_is_set(LED_LEFT);
  report->leds.right     = pin_is_set(LED_RIGHT);
  report->leds.onboard   = pin_is_set(LED_BUILTIN);
  report->motors.left    = motor_power_left;
  report->motors.right   = motor_power_right;
  report->sensors.left   = adc_values[ADC_SENSOR_LEFT];
  report->sensors.center = adc_values[ADC_SENSOR_CENTER];
  report->sensors.right  = adc_values[ADC_SENSOR_RIGHT];
  return true;
}
