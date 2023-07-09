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

// The report to be sent.
static report_t report;

// report_init initializes the report module.
void report_init() {}

// report_send sends the report, if usart0 is ready.
void report_send() {
  if (!usart0_write_ready()) {
    return;
  }

  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    if (control_report(&report.data.control)) {
      report.type = REPORT_CONTROL;
    } else if (platform_report(&report.data.platform)) {
      report.type = REPORT_PLATFORM;
    } else {
      report.type = REPORT_NONE;
    }
  }

  if (report.type == REPORT_NONE) {
    return;
  }

  report.rtc.micros = rtc_micros();
  usart0_write((uint8_t*)&report, sizeof(report_t));
}
