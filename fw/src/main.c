#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/delay.h>

#include "adc.h"
#include "pin.h"
#include "report.h"
#include "timer1.h"
#include "usart0.h"

int main() {
  pin_init();
  usart0_init();
  timer1_init();
  report_init();
  adc_init();
  sei();

  for (;;) {
    timer1_wait();

    if (report_available()) {
      report.ir_sensor_left = adc_read_0();
      report_send();
    }

    pin_toggle(LED_BUILTIN);
  }

  return 0;
}
