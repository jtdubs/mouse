#include <avr/interrupt.h>

#include "control/control.h"
#include "control/plan.h"
#include "modes/remote.h"
#include "platform/platform.h"
#include "serial/serial.h"

int main() {
  platform_init();
  serial_init();
  plan_init();
  control_init();
  sei();

  // we are remote-controlled
  remote();

  return 0;
}
