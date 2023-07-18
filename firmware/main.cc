#include <avr/interrupt.h>
// #include <stddef.h>

#include "firmware/lib/control/control.hh"
#include "firmware/lib/maze/maze.hh"
#include "firmware/lib/mode/remote/remote.hh"
#include "firmware/lib/report/report.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/platform/platform.hh"

// void *operator new(size_t)                            = delete;
// void *operator new[](size_t)                          = delete;
// void operator delete(void *) noexcept                 = delete;
// void operator delete[](void *) noexcept               = delete;
// void operator delete(void *, unsigned int) noexcept   = delete;
// void operator delete[](void *, unsigned int) noexcept = delete;

int main() {
  platform::Init();  // hardware peripherals
  control::Init();   // control planning
  maze::Init();      // maze data structure
  report::Init();    // serial report system

  // enable interrupts
  sei();

  // remote control mode (never returns)
  remote::Run();

  // seriously... it should never return.
  assert::failed(assert::MAIN + 0);

  return 0;
}
