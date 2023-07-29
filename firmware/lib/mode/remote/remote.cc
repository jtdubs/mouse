#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "command.hh"
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
dequeue::Dequeue<plan::Plan, 16> plans;
}

// remote is a mode that allows the robot to be controlled remotely.
void Run() {
  command::Init();
  plan::SubmitAndWait((plan::Plan){.type = plan::Type::Idle, .state = plan::State::Scheduled, .data = {.idle = {}}});

  for (;;) {
    // wait until there's a command to process.
    command::Command command;
    while (!command::Next(command)) {}

    switch (command.type) {
      case command::Type::Reset:
        // Just turn off interrupts and wait for the watchdog to reset us.
        cli();
        _delay_ms(60000.0);
        break;
      case command::Type::Explore:
        explore::Explore();
        break;
      case command::Type::Solve:
        explore::Solve();
        break;
      case command::Type::SendMaze:
        maze::Send();
        break;
      case command::Type::TunePID:
        switch (command.data.pid.id) {
          case command::PidID::Speed:
            speed::TunePID(command.data.pid.kp,  //
                           command.data.pid.ki,  //
                           command.data.pid.kd,  //
                           command.data.pid.alpha);
            break;
          case command::PidID::Wall:
            linear::TuneWallPID(command.data.pid.kp,  //
                                command.data.pid.ki,  //
                                command.data.pid.kd,  //
                                command.data.pid.alpha);
            break;
          case command::PidID::Angle:
            linear::TuneAnglePID(command.data.pid.kp,  //
                                 command.data.pid.ki,  //
                                 command.data.pid.kd,  //
                                 command.data.pid.alpha);
            break;
        }
        break;
      case command::Type::PlanEnqueue:
        plans.PushBack(command.data.plan);
        break;
      case command::Type::PlanExecute:
        while (!plans.Empty()) {
          plan::SubmitAndWait(plans.PopFront());
        }
        break;
      default:
        break;
    }

    command::Processed();
  }
}

}  // namespace remote
