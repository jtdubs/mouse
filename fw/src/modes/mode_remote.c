#include "modes/mode_remote.h"

#include "platform/motor.h"
#include "platform/pin.h"
#include "serial/command.h"

// mode_remote_tick is the remote mode's tick function.
void mode_remote_tick() {
  // If a serial command has been received, process it.
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
      case COMMAND_SET_LEFT_MOTOR_FORWARD:
        set_left_motor_forward(command.value > 0);
        break;
      case COMMAND_SET_RIGHT_MOTOR_FORWARD:
        set_right_motor_forward(command.value > 0);
        break;
      default:
        return;
    }
    command_processed();
  }
}
