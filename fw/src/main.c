#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/delay.h>

#include "adc.h"
#include "battery.h"
#include "command.h"
#include "fsel.h"
#include "pin.h"
#include "report.h"
#include "timer1.h"
#include "usart0.h"

void init() {
  pin_init();
  usart0_init();
  timer1_init();
  report_init();
  command_init();
  adc_init();
  fsel_init();
  battery_init();
  sei();
}

void tick() {
  fsel_update();
  battery_update();

  // pin_toggle(RIGHT_PWM);

  if (report_available()) {
    report.battery_volts   = battery_voltage;
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

int main() {
  init();
  for (;;) {
    timer1_wait();
    tick();
  }
  return 0;
}
