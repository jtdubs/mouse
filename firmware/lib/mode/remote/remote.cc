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

namespace mouse::mode::remote {

namespace {
dequeue::Dequeue<control::plan::Plan, 4> plans_;
}

// remote is a mode that allows the robot to be controlled remotely.
void Run() {
  Init();
  control::plan::SubmitAndWait((control::plan::Plan){
      .type = control::plan::Type::Idle, .state = control::plan::State::Scheduled, .data = {.idle = {}}});

  for (;;) {
    // wait until there's a command to process.
    Command command;
    while (!Next(command)) {}

    switch (command.type) {
      case Type::Reset:
        // Just turn off interrupts and wait for the watchdog to reset us.
        cli();
        _delay_ms(60000.0);
        break;
      case Type::Explore:
        explore::Explore();
        break;
      case Type::Solve:
        explore::Solve();
        break;
      case Type::SendMaze:
        maze::Send();
        break;
      case Type::TunePID:
        switch (command.data.pid.id) {
          case PidID::Speed:
            control::speed::TunePID(command.data.pid.kp,  //
                                    command.data.pid.ki,  //
                                    command.data.pid.kd,  //
                                    command.data.pid.alpha);
            break;
          case PidID::Wall:
            control::linear::TuneWallPID(command.data.pid.kp,  //
                                         command.data.pid.ki,  //
                                         command.data.pid.kd,  //
                                         command.data.pid.alpha);
            break;
          case PidID::Angle:
            control::linear::TuneAnglePID(command.data.pid.kp,  //
                                          command.data.pid.ki,  //
                                          command.data.pid.kd,  //
                                          command.data.pid.alpha);
            break;
        }
        break;
      case Type::PlanEnqueue:
        plans_.PushBack(command.data.plan);
        break;
      case Type::PlanExecute:
        while (!plans_.Empty()) {
          control::plan::SubmitAndWait(plans_.PopFront());
        }
        break;
      default:
        break;
    }

    Processed();
  }
}

}  // namespace mouse::mode::remote
