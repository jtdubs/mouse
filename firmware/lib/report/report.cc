#include <stddef.h>
#include <stdint.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "report_impl.hh"

namespace mouse::report {

namespace {
// The report to be sent.
Report report_;
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

  report_.rtc_micros = platform::rtc::Micros();
  report_.length     = 0;
  if (len = mode::explore::GetReport(report_.data, sizeof(report_.data)); len > 0) {
    report_.type   = Type::Explore;
    report_.length = len;
  } else if (len = maze::GetReport(report_.data, sizeof(report_.data)); len > 0) {
    report_.type   = Type::Maze;
    report_.length = len;
  } else if (len = control::GetReport(report_.data, sizeof(report_.data)); len > 0) {
    report_.type   = Type::Control;
    report_.length = len;
  } else if (len = platform::GetReport(report_.data, sizeof(report_.data)); len > 0) {
    report_.type   = Type::Platform;
    report_.length = len;
  }

  if (report_.length == 0) {
    return;
  }

  platform::usart0::Write((uint8_t*)&report_, report_.length + 6);
}

}  // namespace mouse::report
