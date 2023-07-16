#include <avr/interrupt.h>

#include "firmware/lib/control/control.h"
#include "firmware/lib/maze/maze.h"
#include "firmware/lib/mode/explore/explore.h"
#include "firmware/lib/mode/remote/remote.h"
#include "firmware/lib/report/report.h"
#include "firmware/lib/utils/assert.h"
#include "firmware/platform/platform.h"

int main() {
  platform_init();  // hardware peripherals
  control_init();   // control planning
  maze_init();      // maze data structure
  explore_init();   // maze exploration mode
  report_init();    // serial report system

  // enable interrupts
  sei();

  // remote control mode (never returns)
  remote();

  // seriously... it should never return.
  _assert_failed(ASSERT_MAIN + 0);

  return 0;
}
