//
// System: serial
// Module: command
//
// Purpose:
// - Defines the data structure used to receive serial commands from the host.
// - Provides functions to determine if a command is available, and to indicate
//   when it has been processed.
// - Receives callbacks from the usart0 module when a command is received.
// - Manages the state of the usart0 receiver, based on whether or not a command
//   is already in the buffer waiting to be processed.
//
// Dependencies:
// - Uses the usart0 module to receive commands.
//
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "firmware/lib/control/plan.hh"

namespace remote::command {

// type_t identifies the type of a given command.
enum class Type : uint8_t {
  Reset,
  Explore,
  Solve,
  SendMaze,
  TunePID,
  PlanEnqueue,
  PlanExecute,
};

// pid_id_t identifies the PID to tune.
enum class PidID : uint8_t {
  Speed,
  Wall,
  Angle,
};

#pragma pack(push, 1)
// Command represents a command that can be processed by the mouse.
struct Command {
  Type type;
  union {
    struct {
      bool left;
      bool right;
      bool onboard;
      bool ir;
    } leds;
    struct {
      PidID id;
      float kp;
      float ki;
      float kd;
      float alpha;
    } pid;
    plan::Plan plan;
  } data;
};
#pragma pack(pop)

// Init initializes the command module.
void Init();

// processed indicates the command has been processed.
void processed();

// next gets the next command, if one is available.
bool next(Command &command);

}  // namespace remote::command
