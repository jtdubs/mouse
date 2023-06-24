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
        set_left_motor_speed(command.data.motors.left_speed);
        set_right_motor_speed(command.data.motors.right_speed);
        set_left_motor_forward(command.data.motors.left_forward);
        set_right_motor_forward(command.data.motors.right_forward);
        break;
      default:
        return;
    }
    command_processed();
  }
}
