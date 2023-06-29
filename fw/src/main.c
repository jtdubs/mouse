#include <avr/interrupt.h>
#include <avr/power.h>
#include <stddef.h>

#include "control/speed.h"
#include "modes/mode.h"
#include "platform/adc.h"
#include "platform/encoders.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "platform/rtc.h"
#include "platform/timer.h"
#include "platform/usart0.h"
#include "serial/command.h"
#include "serial/report.h"
#include "utils/sim.h"

static void init() {
  // Turn off unused hardware to save a few mA.
  power_spi_disable();
  power_twi_disable();
  power_timer2_disable();

  // Initialize all the modules.
  pin_init();
  usart0_init();
  adc_init();
  encoders_init();
  motor_init();
  report_init();
  command_init();
  speed_init();
  mode_init();
  timer_init();
  rtc_init();

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
