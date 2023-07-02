#include "serial/report.h"

#include <stddef.h>
#include <stdint.h>
#include <util/atomic.h>

#include "control/position.h"
#include "control/speed.h"
#include "platform/adc.h"
#include "platform/encoders.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "platform/rtc.h"
#include "platform/usart0.h"
#include "utils/assert.h"

// The un-encoded and encoded report.
report_t report;

// report_init initializes the report module.
void report_init() {}

// report_send sends the report, if usart0 is ready.
void report_send() {
  if (usart0_write_ready()) {
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
      // report.battery_volts        = adc_values[ADC_BATTERY_VOLTAGE] >> 2;
      // report.sensors.left         = adc_values[ADC_SENSOR_LEFT];
      // report.sensors.center       = adc_values[ADC_SENSOR_CENTER];
      // report.sensors.right        = adc_values[ADC_SENSOR_RIGHT];
      report.leds.onboard         = pin_is_set(LED_BUILTIN);
      report.leds.left            = pin_is_set(LED_LEFT);
      report.leds.right           = pin_is_set(LED_RIGHT);
      report.leds.ir              = pin_is_set(IR_LEDS);
      report.encoders.left        = encoders_left;
      report.encoders.right       = encoders_right;
      report.motors.left          = motor_power_left;
      report.motors.right         = motor_power_right;
      report.speed.measured_left  = speed_measured_left;
      report.speed.measured_right = speed_measured_right;
      report.speed.setpoint_left  = speed_setpoint_left;
      report.speed.setpoint_right = speed_setpoint_right;
      report.position.distance    = position_distance;
      report.position.theta       = position_theta;
      report.plan                 = current_plan;
      report.rtc.micros           = rtc_micros();
    }
    usart0_write((uint8_t*)&report, sizeof(report_t));
  }
}
