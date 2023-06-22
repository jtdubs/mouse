#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <stddef.h>
#include <util/delay.h>

#include "modes/mode.h"
#include "platform/adc.h"
#include "platform/encoders.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "platform/timer.h"
#include "platform/usart0.h"
#include "serial/command.h"
#include "serial/report.h"
#include "utils/sim.h"

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

  power_spi_disable();
  power_twi_disable();
  power_timer2_disable();

  wdt_enable(WDTO_15MS);

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
    wdt_reset();
    tick();
  }

  return 0;
}
