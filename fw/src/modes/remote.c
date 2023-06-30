#include "modes/remote.h"

#include <util/delay.h>

#include "control/plan.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "serial/command.h"

// remote is a mode that allows the robot to be controlled remotely.
void remote() {
  pin_clear(LED_BUILTIN);
  pin_clear(LED_LEFT);
  pin_clear(LED_RIGHT);
  pin_clear(IR_LEDS);

  plan_submit_and_wait(                          //
      &(plan_t){.type       = PLAN_FIXED_POWER,  //
                .data.power = {0, 0}});

  for (;;) {
    if (!command_available()) {
      continue;
    }

    switch (command->type) {
      case COMMAND_SET_LEDS:
        pin_set2(LED_BUILTIN, command->data.leds.builtin);
        pin_set2(LED_LEFT, command->data.leds.left);
        pin_set2(LED_RIGHT, command->data.leds.right);
        pin_set2(IR_LEDS, command->data.leds.ir);
        break;
      case COMMAND_SET_POWER:
        plan_submit_and_wait(  //
            &(plan_t){.type       = PLAN_FIXED_POWER,
                      .data.power = {.left  = command->data.power.left,  //
                                     .right = command->data.power.right}});
        break;
      case COMMAND_SET_SPEED:
        plan_submit_and_wait(  //
            &(plan_t){.type       = PLAN_FIXED_SPEED,
                      .data.speed = {.left  = command->data.speed.left,  //
                                     .right = command->data.speed.right}});
        break;
      default:
        break;
    }

    command_processed();
  }
}
