#include <avr/interrupt.h>

#include "firmware/lib/control/control.hh"
#include "firmware/lib/maze/maze.hh"
#include "firmware/lib/mode/remote/remote.hh"
#include "firmware/lib/report/report.hh"
#include "firmware/lib/utils/assert.hh"
#include "firmware/platform/platform.hh"

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
  assert::Failed(assert::Module::Main, 0);

  return 0;
}
