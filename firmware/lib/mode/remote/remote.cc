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
#include "firmware/platform/platform.hh"
#include "remote_impl.hh"

namespace remote {

namespace {
dequeue::dequeue<plan::plan_t, 16> plans;
}

// remote is a mode that allows the robot to be controlled remotely.
void remote() {
  command::init();
  plan::submit_and_wait(
      (plan::plan_t){.type = plan::Type::Idle, .state = plan::State::Scheduled, .data = {.idle = {}}});

  for (;;) {
    // wait until there's a command to process.
    command::command_t command;
    while (!command::next(command)) {}

    switch (command.type) {
      case command::Type::Reset:
        // Just turn off interrupts and wait for the watchdog to reset us.
        cli();
        _delay_ms(60000.0);
        break;
      case command::Type::Explore:
        explore::explore();
        break;
      case command::Type::Solve:
        explore::solve();
        break;
      case command::Type::SendMaze:
        maze::send();
        break;
      case command::Type::TunePID:
        switch (command.data.pid.id) {
          case command::PidID::Speed:
            speed::tune(command.data.pid.kp,  //
                        command.data.pid.ki,  //
                        command.data.pid.kd,  //
                        command.data.pid.alpha);
            break;
          case command::PidID::Wall:
            linear::wall_tune(command.data.pid.kp,  //
                              command.data.pid.ki,  //
                              command.data.pid.kd,  //
                              command.data.pid.alpha);
            break;
          case command::PidID::Angle:
            linear::angle_tune(command.data.pid.kp,  //
                               command.data.pid.ki,  //
                               command.data.pid.kd,  //
                               command.data.pid.alpha);
            break;
        }
        break;
      case command::Type::PlanEnqueue:
        plans.push_back(command.data.plan);
        break;
      case command::Type::PlanExecute:
        while (!plans.empty()) {
          plan::submit_and_wait(plans.pop_front());
        }
        break;
      default:
        break;
    }

    command::processed();
  }
}

}  // namespace remote
