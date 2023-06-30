#include "serial/report.h"

#include <stddef.h>
#include <stdint.h>
#include <util/atomic.h>

#include "control/position.h"
#include "control/speed.h"
#include "modes/mode.h"
#include "modes/mode_wall.h"
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
      // report.battery_volts        = adc_battery_voltage >> 2;
      // report.mode                 = mode_active;
      report.encoders.left        = encoders_left;
      report.encoders.right       = encoders_right;
      report.motors.left          = motor_forward_left ? motor_power_left : -motor_power_left;
      report.motors.right         = motor_forward_right ? motor_power_right : -motor_power_right;
      report.speed.measured_left  = speed_measured_left;
      report.speed.measured_right = speed_measured_right;
      report.speed.setpoint_left  = speed_setpoint_left;
      report.speed.setpoint_right = speed_setpoint_right;
      report.speed.kp             = speed_pid_left.kp;
      report.speed.ki             = speed_pid_left.ki;
      report.speed.kd             = speed_pid_left.kd;
      // report.position.measured_left  = position_measured_left;
      // report.position.measured_right = position_measured_right;
      // report.position.setpoint_left  = position_setpoint_left;
      // report.position.setpoint_right = position_setpoint_right;
      // report.sensors.left   = adc_sensor_left;
      // report.sensors.center = adc_sensor_center;
      // report.sensors.right  = adc_sensor_right;
      // report.leds.onboard   = pin_is_set(LED_BUILTIN);
      // report.leds.left      = pin_is_set(LED_LEFT);
      // report.leds.right     = pin_is_set(LED_RIGHT);
      // report.leds.ir        = pin_is_set(IR_LEDS);
      report.rtc.micros = rtc_micros();
    }
    usart0_write((uint8_t*)&report, sizeof(report_t));
  }
}
