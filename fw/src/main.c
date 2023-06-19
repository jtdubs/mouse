#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/delay.h>

#include "adc.h"
#include "battery.h"
#include "command.h"
#include "encoders.h"
#include "fsel.h"
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
      case COMMAND_SET_ONBOARD_LED:
        pin_set2(LED_BUILTIN, command.value > 0);
        break;
      case COMMAND_SET_LEFT_LED:
        pin_set2(LED_LEFT, command.value > 0);
        break;
      case COMMAND_SET_RIGHT_LED:
        pin_set2(LED_RIGHT, command.value > 0);
        break;
      case COMMAND_SET_IR_LEDS:
        pin_set2(IR_LEDS, command.value > 0);
        break;
      case COMMAND_SET_LEFT_MOTOR_SPEED:
        set_left_motor_speed(command.value);
        break;
      case COMMAND_SET_RIGHT_MOTOR_SPEED:
        set_right_motor_speed(command.value);
        break;
      case COMMAND_SET_LEFT_MOTOR_DIR:
        set_left_motor_dir(command.value > 0);
        break;
      case COMMAND_SET_RIGHT_MOTOR_DIR:
        set_right_motor_dir(command.value > 0);
        break;
    }
    command_processed();
  }
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
