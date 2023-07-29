#include <avr/interrupt.h>

#include "firmware/lib/control/control.hh"
#include "firmware/lib/maze/maze.hh"
#include "firmware/lib/mode/remote/remote.hh"
#include "firmware/lib/report/report.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/platform/platform.hh"

int main() {
  mouse::platform::Init();  // hardware peripherals
  mouse::control::Init();   // control planning
  mouse::maze::Init();      // maze data structure
  mouse::report::Init();    // serial report system

  // enable interrupts
  sei();

  // remote control mode (never returns)
  mouse::mode::remote::Run();

  // seriously... it should never return.
  mouse::assert::Failed(mouse::assert::Module::Main, 0);

  return 0;
}
