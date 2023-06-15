#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/delay.h>

#include "adc.h"
#include "battery.h"
#include "command.h"
#include "encoders.h"
#include "fsel.h"
#include "pin.h"
#include "pwm.h"
#include "report.h"
#include "timer.h"
#include "usart0.h"

void init() {
  pin_init();
  usart0_init();
  timer_init();
  report_init();
  command_init();
  adc_init();
  fsel_init();
  battery_init();
  encoders_init();
  pwm_init();
  sei();
}

void tick() {
  fsel_update();
  battery_update();

  if (report_available()) {
    report.battery_volts   = battery_voltage;
    report.function_select = fsel;
    report.encoder_left    = encoder_left;
    report.encoder_right   = encoder_right;
    report_send();
  }

  if (command_available()) {
    switch (command.type) {
      case COMMAND_LED:
        if (command.value == 0) {
          pin_clear(LED_BUILTIN);
        } else {
          pin_set(LED_BUILTIN);
        }
        break;
      case COMMAND_PWM_LEFT:
        set_pwm_duty_cycle_a(command.value);
        break;
      case COMMAND_PWM_RIGHT:
        set_pwm_duty_cycle_b(command.value);
        break;
    }
    command_processed();
  }
}

int main() {
  init();
  for (;;) {
    timer_wait();
    tick();
  }
  return 0;
}
