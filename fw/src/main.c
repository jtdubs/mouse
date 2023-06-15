#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/delay.h>

#include "adc.h"
#include "command.h"
#include "fsel.h"
#include "pin.h"
#include "report.h"
#include "timer1.h"
#include "usart0.h"

int main() {
  pin_init();
  usart0_init();
  timer1_init();
  report_init();
  command_init();
  adc_init();
  fsel_init();
  sei();

  for (;;) {
    timer1_wait();

    fsel_update();

    if (report_available()) {
      report.battery_volts   = adc_read(7) >> 2;
      report.function_select = fsel;
      report_send();
    }

    if (command_available()) {
      if (command.led == 0) {
        pin_clear(LED_BUILTIN);
      } else {
        pin_set(LED_BUILTIN);
      }

      command_processed();
    }
  }

  return 0;
}
