#include "report.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "adc.h"
#include "base64.h"
#include "encoders.h"
#include "mode_wall.h"
#include "pin.h"
#include "usart0.h"

#define ENCODED_SIZE ((sizeof(report_t) * 4 / 3) + 3)

report_t    report;
static char encoded_report[ENCODED_SIZE];

// report_init initializes the report module.
void report_init() {
  assert(sizeof(report_t) % 3 == 0);

  encoded_report[0]                = '[';
  encoded_report[ENCODED_SIZE - 2] = ']';
  encoded_report[ENCODED_SIZE - 1] = '\n';

  usart0_set_write_buffer((uint8_t*)encoded_report, ENCODED_SIZE);
}

// report_send sends the report.
void report_send() {
  if (report_available()) {
    report.battery_volts   = battery_voltage;
    report.mode            = active_mode;
    report.encoder_left    = encoder_left;
    report.encoder_right   = encoder_right;
    report.sensors.left    = sensor_left;
    report.sensors.center  = sensor_center;
    report.sensors.right   = sensor_right;
    report.distance_left   = distance_left;
    report.distance_center = distance_center;
    report.distance_right  = distance_right;
    report.leds.onboard    = pin_is_set(LED_BUILTIN);
    report.leds.left       = pin_is_set(LED_LEFT);
    report.leds.right      = pin_is_set(LED_RIGHT);
    report.leds.ir         = pin_is_set(IR_LEDS);
    base64_encode((uint8_t*)&report, (uint8_t*)&encoded_report[1], sizeof(report));
    usart0_write();
  }
}
