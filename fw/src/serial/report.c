#include "serial/report.h"

#include <stddef.h>
#include <stdint.h>

#include "modes/mode.h"
#include "modes/mode_wall.h"
#include "platform/adc.h"
#include "platform/encoders.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "platform/usart0.h"
#include "utils/assert.h"
#include "utils/base64.h"

#define ENCODED_SIZE ((sizeof(report_t) * 4 / 3) + 3)

// The un-encoded and encoded report.
report_t    report;
static char encoded_report[ENCODED_SIZE];

// report_init initializes the report module.
void report_init() {
  // Ensure the report_t is a multiple of 3 bytes, as that encodes easily into base64 w/o padding.
  _Static_assert((sizeof(report_t) % 3) == 0);

  encoded_report[0]                = '[';
  encoded_report[ENCODED_SIZE - 2] = ']';
  encoded_report[ENCODED_SIZE - 1] = '\n';

  usart0_set_write_buffer((uint8_t*)encoded_report, ENCODED_SIZE);
}

// report_send sends the report, if usart0 is ready.
void report_send() {
  if (usart0_write_ready()) {
    report.battery_volts        = adc_battery_voltage >> 2;
    report.mode                 = mode_active;
    report.motors.encoder_left  = encoder_left;
    report.motors.encoder_right = encoder_right;
    report.motors.forward_left  = motor_left_forward ? 1 : 0;
    report.motors.forward_right = motor_right_forward ? 1 : 0;
    report.motors.speed_left    = motor_left_speed;
    report.motors.speed_right   = motor_right_speed;
    report.sensors.left         = adc_sensor_left;
    report.sensors.center       = adc_sensor_center;
    report.sensors.right        = adc_sensor_right;
    report.leds.onboard         = pin_is_set(LED_BUILTIN);
    report.leds.left            = pin_is_set(LED_LEFT);
    report.leds.right           = pin_is_set(LED_RIGHT);
    report.leds.ir              = pin_is_set(IR_LEDS);
    base64_encode((uint8_t*)&report, (uint8_t*)&encoded_report[1], sizeof(report));
    usart0_write();
  }
}
