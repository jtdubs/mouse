#include "modes/remote.h"

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "control/linear.h"
#include "control/plan.h"
#include "control/speed.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "serial/command.h"

static plan_t  remote_plan_queue[16];
static uint8_t remote_plan_queue_size = 0;

static void remote_enqueue(plan_t plan) {
  if (remote_plan_queue_size < 16) {
    remote_plan_queue[remote_plan_queue_size++] = plan;
  }
}

// remote is a mode that allows the robot to be controlled remotely.
void remote() {
  set_sleep_mode(SLEEP_MODE_IDLE);

  plan_submit_and_wait(                       //
      &(plan_t){.type      = PLAN_TYPE_LEDS,  //
                .data.leds = {false, false, false}});
  plan_submit_and_wait(                   //
      &(plan_t){.type    = PLAN_TYPE_IR,  //
                .data.ir = {false}});
  plan_submit_and_wait(                               //
      &(plan_t){.type       = PLAN_TYPE_FIXED_POWER,  //
                .data.power = {0, 0}});

  for (;;) {
    while (!command_available) {
      sleep_mode();
    }

    switch (command->type) {
      case COMMAND_RESET:
        // Just turn off interrupts and wait for the watchdog to reset us.
        cli();
        _delay_ms(60000.0);
        break;
      case COMMAND_SET_SPEED_PID:
        speed_set_pi_coefficients(command->data.pid.kp,  //
                                  command->data.pid.ki,  //
                                  command->data.pid.alpha);
        break;
      case COMMAND_PLAN_ENQUEUE:
        remote_enqueue(command->data.plan);
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
