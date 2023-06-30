#include <avr/interrupt.h>

#include "control/control.h"
#include "modes/remote.h"
#include "platform/platform.h"
#include "serial/serial.h"

int main() {
  platform_init();
  serial_init();
  control_init();
  sei();

  for (;;) {
    remote();
  }

  return 0;
}
