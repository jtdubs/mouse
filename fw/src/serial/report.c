#include "serial/report.h"

#include <stddef.h>
#include <stdint.h>
#include <util/atomic.h>

#include "control/control.h"
#include "maze/maze.h"
#include "modes/explore.h"
#include "platform/platform.h"
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

  uint8_t len = 0;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if ((len = explore_report(report.data, sizeof(report.data))) > 0) {
      report.header.type = REPORT_EXPLORE;
    } else if ((len = maze_report(report.data, sizeof(report.data))) > 0) {
      report.header.type = REPORT_MAZE;
    } else if ((len = control_report(report.data, sizeof(report.data))) > 0) {
      report.header.type = REPORT_CONTROL;
    } else if ((len = platform_report(report.data, sizeof(report.data))) > 0) {
      report.header.type = REPORT_PLATFORM;
    }
  }

  if (len == 0) {
    return;
  }

  report.header.rtc_micros = rtc_micros();
  usart0_write((uint8_t*)&report, sizeof(report_header_t) + len);
}
