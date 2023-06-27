#include "modes/mode_remote.h"

#include "platform/motor.h"
#include "platform/pin.h"
#include "serial/command.h"

// mode_remote_tick is the remote mode's tick function.
void mode_remote_tick() {
  // If a serial command has been received, process it.
  if (command_available()) {
    switch (command.type) {
      case COMMAND_SET_LEDS:
        pin_set2(LED_BUILTIN, command.data.leds.builtin > 0);
        pin_set2(LED_LEFT, command.data.leds.left > 0);
        pin_set2(LED_RIGHT, command.data.leds.right > 0);
        pin_set2(IR_LEDS, command.data.leds.ir > 0);
        break;
      case COMMAND_SET_MOTORS:
        motor_set_speed_left(command.data.motors.speed_left);
        motor_set_speed_right(command.data.motors.speed_right);
        motor_set_forward_left(command.data.motors.forward_left);
        motor_set_forward_right(command.data.motors.forward_right);
        break;
      default:
        return;
    }
    command_processed();
  }
}
