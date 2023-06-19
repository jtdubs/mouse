#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/delay.h>

#include "adc.h"
#include "battery.h"
#include "command.h"
#include "encoders.h"
#include "mode.h"
#include "motor.h"
#include "pin.h"
#include "report.h"
#include "sensor.h"
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

void tick_remote();
void tick_wall();

void tick() {
  battery_update();
  sensor_update();

  switch (mode.active) {
    case MODE_REMOTE:
      tick_remote();
      break;
    case MODE_WALL:
      tick_wall();
      break;
    default:
      break;
  }

  report_send();

  mode_update();
  if (command_available() && command.type == COMMAND_SET_MODE) {
    if (command.value < COMMAND_MAX_VALUE) {
      mode.active = command.value;
    }
    command_processed();
  }
}

void tick_remote() {
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
      default:
        return;
    }
    command_processed();
  }
}

void tick_wall() {
  pin_set2(LED_LEFT, sensor_left > 100);
  pin_set2(LED_BUILTIN, sensor_center > 100);
  pin_set2(LED_RIGHT, sensor_right > 100);
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
