#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/delay.h>

#include "adc.h"
#include "command.h"
#include "encoders.h"
#include "mode.h"
#include "motor.h"
#include "pin.h"
#include "report.h"
#include "sim.h"
#include "timer.h"
#include "usart0.h"

void init() {
  pin_init();
  usart0_init();
  timer_init();
  adc_init();
  encoders_init();
  motor_init();
  report_init();
  command_init();
  mode_init();
  sei();
}

void tick() {
  mode_tick();
  report_send();
  mode_update();
}

int main() {
  sim_start_trace();

  init();
  for (;;) {
    timer_wait();
    tick();
  }

  return 0;
}
