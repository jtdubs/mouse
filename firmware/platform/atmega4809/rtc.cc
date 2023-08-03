#include <avr/io.h>
#include <util/atomic.h>

#include "rtc_impl.hh"

namespace mouse::platform::rtc {

namespace {
// overflow_count is the number of times the timer has overflowed.
uint32_t overflow_count_;
}  // namespace

// Init initializes the real-time clock.
void Init() {
  // TODO
  overflow_count_ = 0;
}

// micros returns the number of microseconds since RTC initialization.
uint32_t Micros() {
  // TODO
  return 0;
}

}  // namespace mouse::platform::rtc
