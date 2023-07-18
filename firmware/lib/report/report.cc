#include <stddef.h>
#include <stdint.h>
#include <util/atomic.h>

#include "firmware/lib/control/control.hh"
#include "firmware/lib/maze/maze.hh"
#include "firmware/lib/mode/explore/explore.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/platform/platform.hh"
#include "firmware/platform/rtc.hh"
#include "firmware/platform/timer.hh"
#include "firmware/platform/usart0.hh"
#include "report_impl.hh"

namespace report {

namespace {
// The report to be sent.
report_t report;
}  // namespace

// init initializes the report module.
void init() {
  timer::add_callback(send);
}

// send sends the report, if usart0 is ready.
void send() {
  if (!usart0::write_ready()) {
    return;
  }

  uint8_t len              = 0;
  report.header.rtc_micros = rtc::micros();
  if ((len = explore::report(report.data, sizeof(report.data))) > 0) {
    report.header.type = EXPLORE;
  } else if ((len = maze::report(report.data, sizeof(report.data))) > 0) {
    report.header.type = MAZE;
  } else if ((len = control::report(report.data, sizeof(report.data))) > 0) {
    report.header.type = CONTROL;
  } else if ((len = platform::report(report.data, sizeof(report.data))) > 0) {
    report.header.type = PLATFORM;
  }

  if (len == 0) {
    return;
  }

  usart0::write((uint8_t*)&report, sizeof(report_header_t) + len);
}

}  // namespace report
