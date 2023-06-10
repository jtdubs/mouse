#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/delay.h>

#include "pin.h"
#include "report.h"
#include "timer1.h"
#include "usart0.h"

int main() {
  pin_init();
  usart0_init();
  timer1_init();
  report_init();
  sei();

  for (;;) {
    timer1_wait();

    if (report_available()) {
      report.ir_sensor_left = 0x1234;
      report_send();
    }

    pin_toggle(LED_BUILTIN);
  }

  return 0;
}
