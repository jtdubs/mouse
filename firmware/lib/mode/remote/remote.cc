#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "command_impl.hh"
#include "firmware/lib/control/linear.hh"
#include "firmware/lib/control/plan.hh"
#include "firmware/lib/control/speed.hh"
#include "firmware/lib/maze/maze.hh"
#include "firmware/lib/mode/explore/explore.hh"
#include "firmware/lib/utils/dequeue.hh"
#include "firmware/platform/motor.hh"
#include "firmware/platform/pin.hh"
#include "remote_impl.hh"

namespace remote {

namespace {
DEFINE_DEQUEUE(plan::plan_t, plans, 16);
}

// remote is a mode that allows the robot to be controlled remotely.
void remote() {
  command::init();
  plans_init();
  plan::submit_and_wait((plan::plan_t){.type = plan::TYPE_IDLE, .state = plan::STATE_SCHEDULED, .data = {.idle = {}}});

  for (;;) {
    // wait until there's a command to process.
    command::command_t command;
    while (!command::next(&command)) {}

    switch (command.type) {
      case command::RESET:
        // Just turn off interrupts and wait for the watchdog to reset us.
        cli();
        _delay_ms(60000.0);
        break;
      case command::EXPLORE:
        explore::explore();
        break;
      case command::SOLVE:
        explore::solve();
        break;
      case command::SEND_MAZE:
        maze::send();
        break;
      case command::TUNE_PID:
        switch (command.data.pid.id) {
          case command::PID_SPEED:
            speed::tune(command.data.pid.kp,  //
                        command.data.pid.ki,  //
                        command.data.pid.kd,  //
                        command.data.pid.alpha);
            break;
          case command::PID_WALL:
            linear::wall_tune(command.data.pid.kp,  //
                              command.data.pid.ki,  //
                              command.data.pid.kd,  //
                              command.data.pid.alpha);
            break;
          case command::PID_ANGLE:
            linear::angle_tune(command.data.pid.kp,  //
                               command.data.pid.ki,  //
                               command.data.pid.kd,  //
                               command.data.pid.alpha);
            break;
        }
        break;
      case command::PLAN_ENQUEUE:
        plans_push_back(command.data.plan);
        break;
      case command::PLAN_EXECUTE:
        while (!plans_empty()) {
          plan::submit_and_wait(plans_pop_front());
        }
        break;
      default:
        break;
    }

    command::processed();
  }
}

}  // namespace remote
