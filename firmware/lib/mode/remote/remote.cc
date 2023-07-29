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

namespace mouse::mode::remote {

namespace {
dequeue::Dequeue<control::plan::Plan, 16> plans;
}

// remote is a mode that allows the robot to be controlled remotely.
void Run() {
  command::Init();
  control::plan::SubmitAndWait((control::plan::Plan){
      .type = control::plan::Type::Idle, .state = control::plan::State::Scheduled, .data = {.idle = {}}});

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
            control::speed::TunePID(command.data.pid.kp,  //
                                    command.data.pid.ki,  //
                                    command.data.pid.kd,  //
                                    command.data.pid.alpha);
            break;
          case command::PidID::Wall:
            control::linear::TuneWallPID(command.data.pid.kp,  //
                                         command.data.pid.ki,  //
                                         command.data.pid.kd,  //
                                         command.data.pid.alpha);
            break;
          case command::PidID::Angle:
            control::linear::TuneAnglePID(command.data.pid.kp,  //
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
          control::plan::SubmitAndWait(plans.PopFront());
        }
        break;
      default:
        break;
    }

    command::Processed();
  }
}

}  // namespace mouse::mode::remote
