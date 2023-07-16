#include "remote.h"

#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "command.h"
#include "firmware/lib/control/linear.h"
#include "firmware/lib/control/plan.h"
#include "firmware/lib/control/speed.h"
#include "firmware/lib/maze/maze.h"
#include "firmware/lib/mode/explore/explore.h"
#include "firmware/lib/utils/dequeue.h"
#include "firmware/platform/motor.h"
#include "firmware/platform/pin.h"

DEFINE_DEQUEUE(plan_t, plans, 16);

// remote is a mode that allows the robot to be controlled remotely.
void remote() {
  command_init();
  plans_init();
  plan_submit_and_wait(&(plan_t){.type = PLAN_TYPE_IDLE});

  for (;;) {
    // wait until there's a command to process.
    command_t command;
    while (!command_next(&command)) {}

    switch (command.type) {
      case COMMAND_RESET:
        // Just turn off interrupts and wait for the watchdog to reset us.
        cli();
        _delay_ms(60000.0);
        break;
      case COMMAND_EXPLORE:
        explore();
        break;
      case COMMAND_SOLVE:
        solve();
        break;
      case COMMAND_SEND_MAZE:
        maze_send();
        break;
      case COMMAND_TUNE_PID:
        switch (command.data.pid.id) {
          case PID_SPEED:
            speed_tune(command.data.pid.kp,  //
                       command.data.pid.ki,  //
                       command.data.pid.kd,  //
                       command.data.pid.alpha);
            break;
          case PID_WALL:
            linear_wall_tune(command.data.pid.kp,  //
                             command.data.pid.ki,  //
                             command.data.pid.kd,  //
                             command.data.pid.alpha);
            break;
          case PID_ANGLE:
            linear_angle_tune(command.data.pid.kp,  //
                              command.data.pid.ki,  //
                              command.data.pid.kd,  //
                              command.data.pid.alpha);
            break;
        }
        break;
      case COMMAND_PLAN_ENQUEUE:
        plans_push_back(command.data.plan);
        break;
      case COMMAND_PLAN_EXECUTE:
        while (!plans_empty()) {
          plan_t plan = plans_pop_front();
          plan_submit_and_wait(&plan);
        }
        break;
      default:
        break;
    }

    command_processed();
  }
}
