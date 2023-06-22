#include "mode_remote.h"

#include "command.h"
#include "motor.h"
#include "pin.h"

void mode_remote_tick() {
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
