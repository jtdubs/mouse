#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "control/control.h"
#include "control/plan.h"
#include "maze/maze.h"
#include "modes/remote.h"
#include "platform/platform.h"
#include "serial/serial.h"
#include "utils/assert.h"

int main() {
  set_sleep_mode(SLEEP_MODE_IDLE);

  platform_init();  // hardware peripherals
  serial_init();    // serial report/command system
  control_init();   // control planning
  maze_init();      // maze data structure

  // enable interrupts
  sei();

  // remote control mode (never returns)
  remote();

  // seriously... it should never return.
  _assert_failed(ASSERT_MAIN + 0);

  return 0;
}
