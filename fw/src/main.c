#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>
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

  power_spi_disable();
  power_twi_disable();
  power_timer2_disable();

  // If I switch to Timer2, I can use ADC Noise Reduction mode.
  // set_sleep_mode(SLEEP_MODE_ADC);

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
