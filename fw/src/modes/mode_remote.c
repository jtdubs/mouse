#include "modes/mode_remote.h"

#include "control/position.h"
#include "control/speed.h"
#include "modes/mode.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "serial/command.h"

void mode_remote_enter() {
  mode_enter();
  position_enable();
}

// mode_remote_tick is the remote mode's tick function.
void mode_remote_tick() {
  position_update();

  // If a serial command has been received, process it.
  if (command_available()) {
    switch (command.type) {
      case COMMAND_SET_LEDS:
        pin_set2(LED_BUILTIN, command.data.leds.builtin > 0);
        pin_set2(LED_LEFT, command.data.leds.left > 0);
        pin_set2(LED_RIGHT, command.data.leds.right > 0);
        pin_set2(IR_LEDS, command.data.leds.ir > 0);
        break;
      case COMMAND_SET_SPEED:
        speed_set_left(command.data.speed.left);
        speed_set_right(command.data.speed.right);
        break;
      case COMMAND_SET_POSITION:
        position_set_left(command.data.position.left);
        position_set_right(command.data.position.right);
        break;
      default:
        return;
    }
    command_processed();
  }
}
