#include "modes/remote.h"

#include <util/delay.h>

#include "control/speed.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "serial/command.h"

// remote is a mode that allows the robot to be controlled remotely.
void remote() {
  pin_clear(LED_BUILTIN);
  pin_clear(LED_LEFT);
  pin_clear(LED_RIGHT);
  pin_clear(IR_LEDS);
  motor_set_forward_left(true);
  motor_set_power_left(0);
  motor_set_forward_right(true);
  motor_set_power_right(0);
  speed_enable();

  for (;;) {
    if (!command_available()) {
      _delay_ms(10);
      continue;
    }

    switch (command->type) {
      case COMMAND_SET_LEDS:
        pin_set2(LED_BUILTIN, command->data.leds.builtin > 0);
        pin_set2(LED_LEFT, command->data.leds.left > 0);
        pin_set2(LED_RIGHT, command->data.leds.right > 0);
        pin_set2(IR_LEDS, command->data.leds.ir > 0);
        break;
      case COMMAND_SET_POWER:
        int16_t left          = command->data.power.left;
        int16_t right         = command->data.power.right;
        bool    forward_left  = left > 0;
        bool    forward_right = right > 0;
        if (!forward_left) {
          left = -left;
        }
        if (!forward_right) {
          right = -right;
        }
        speed_disable();
        motor_set_power_left(left);
        motor_set_power_right(right);
        motor_set_forward_left(forward_left);
        motor_set_forward_right(forward_right);
        break;
      case COMMAND_SET_SPEED:
        speed_enable();
        speed_set_left(command->data.speed.left);
        speed_set_right(command->data.speed.right);
        break;
      case COMMAND_SET_SPEED_PI_VARS:
        speed_set_pi_vars(command->data.pid.kp, command->data.pid.ki);
        break;
      default:
        return;
    }
    command_processed();
  }
}
