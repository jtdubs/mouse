#include "modes/remote.h"

#include <avr/sleep.h>
#include <util/delay.h>

#include "control/plan.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "serial/command.h"

static plan_t  remote_plan_queue[16];
static uint8_t remote_plan_queue_size = 0;

static void remote_enqueue(plan_t *plan) {
  if (remote_plan_queue_size < 16) {
    remote_plan_queue[remote_plan_queue_size++] = *plan;
  }
}

// remote is a mode that allows the robot to be controlled remotely.
void remote() {
  set_sleep_mode(SLEEP_MODE_IDLE);

  pin_clear(LED_BUILTIN);
  pin_clear(LED_LEFT);
  pin_clear(LED_RIGHT);
  pin_clear(IR_LEDS);

  plan_submit_and_wait(                               //
      &(plan_t){.type       = PLAN_TYPE_FIXED_POWER,  //
                .data.power = {0, 0}});

  for (;;) {
    while (!command_available()) {
      sleep_mode();
    }

    switch (command->type) {
      case COMMAND_SET_LEDS:
        pin_set2(LED_BUILTIN, command->data.leds.builtin);
        pin_set2(LED_LEFT, command->data.leds.left);
        pin_set2(LED_RIGHT, command->data.leds.right);
        pin_set2(IR_LEDS, command->data.leds.ir);
        break;
      case COMMAND_PLAN_POWER:
        remote_enqueue(  //
            &(plan_t){.type       = PLAN_TYPE_FIXED_POWER,
                      .data.power = {.left  = command->data.power.left,  //
                                     .right = command->data.power.right}});
        break;
      case COMMAND_PLAN_SPEED:
        remote_enqueue(  //
            &(plan_t){.type       = PLAN_TYPE_FIXED_SPEED,
                      .data.speed = {.left  = command->data.speed.left,  //
                                     .right = command->data.speed.right}});
        break;
      case COMMAND_PLAN_LINEAR:
        remote_enqueue(  //
            &(plan_t){.type        = PLAN_TYPE_LINEAR_MOTION,
                      .data.linear = {.distance = command->data.linear.distance,  //
                                      .stop     = command->data.linear.stop}});
        break;
      case COMMAND_PLAN_EXECUTE:
        for (uint8_t i = 0; i < remote_plan_queue_size; i++) {
          plan_submit_and_wait(&remote_plan_queue[i]);
        }
        remote_plan_queue_size = 0;
        break;
      default:
        break;
    }

    command_processed();
  }
}
