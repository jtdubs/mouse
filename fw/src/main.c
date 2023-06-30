#include <avr/interrupt.h>

#include "control/control.h"
#include "modes/mode.h"
#include "platform/platform.h"
#include "serial/serial.h"

static void init() {
  platform_init();
  serial_init();
  control_init();
  mode_init();

  // Enable interrupts.
  sei();
}

static void tick() {
  mode_tick();    // Run the current mode.
  report_send();  // Send a serial report.
  mode_update();  // Update the current mode if necessary.
}

int main() {
  init();
  for (;;) {
    timer_wait();
    tick();
  }

  return 0;
}
