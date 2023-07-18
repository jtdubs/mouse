#include <stddef.h>
#include <stdint.h>
#include <util/atomic.h>

#include "firmware/lib/control/control.hh"
#include "firmware/lib/maze/maze.hh"
#include "firmware/lib/mode/explore/explore.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/platform/platform.hh"
#include "report_impl.hh"

namespace report {

namespace {
// The report to be sent.
Report report;
}  // namespace

// Init initializes the report module.
void Init() {
  timer::AddCallback(Send);
}

// send sends the report, if usart0 is ready.
void Send() {
  if (!usart0::WriteReady()) {
    return;
  }

  uint8_t len              = 0;
  report.header.rtc_micros = rtc::Micros();
  if ((len = explore::GetReport(report.data, sizeof(report.data))) > 0) {
    report.header.type = Type::Explore;
  } else if ((len = maze::GetReport(report.data, sizeof(report.data))) > 0) {
    report.header.type = Type::Maze;
  } else if ((len = control::GetReport(report.data, sizeof(report.data))) > 0) {
    report.header.type = Type::Control;
  } else if ((len = platform::GetReport(report.data, sizeof(report.data))) > 0) {
    report.header.type = Type::Platform;
  }

  if (len == 0) {
    return;
  }

  usart0::Write((uint8_t*)&report, sizeof(ReportHeader) + len);
}

}  // namespace report
