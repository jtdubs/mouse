#include <stddef.h>
#include <stdint.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "report_impl.hh"

namespace mouse::report {

namespace {
// The report to be sent.
Report report;
}  // namespace

// Init initializes the report module.
void Init() {
  platform::timer::AddCallback(Send);
}

// send sends the report, if usart0 is ready.
void Send() {
  if (!platform::usart0::WriteReady()) {
    return;
  }

  uint8_t len;

  report.rtc_micros = platform::rtc::Micros();
  report.length     = 0;
  if (len = mode::explore::GetReport(report.data, sizeof(report.data)); len > 0) {
    report.type   = Type::Explore;
    report.length = len;
  } else if (len = maze::GetReport(report.data, sizeof(report.data)); len > 0) {
    report.type   = Type::Maze;
    report.length = len;
  } else if (len = control::GetReport(report.data, sizeof(report.data)); len > 0) {
    report.type   = Type::Control;
    report.length = len;
  } else if (len = platform::GetReport(report.data, sizeof(report.data)); len > 0) {
    report.type   = Type::Platform;
    report.length = len;
  }

  if (report.length == 0) {
    return;
  }

  platform::usart0::Write((uint8_t*)&report, report.length + 6);
}

}  // namespace mouse::report
