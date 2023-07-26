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

#if not defined(__AVR__)
#include <ostream>
#endif

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
void Processed();

// next gets the next command, if one is available.
bool Next(Command &command);

#if not defined(__AVR__)
std::ostream &operator<<(std::ostream &o, const Type type) {
  switch (type) {
    case Type::Reset:
      o << "Reset";
      break;
    case Type::Explore:
      o << "Explore";
      break;
    case Type::Solve:
      o << "Solve";
      break;
    case Type::SendMaze:
      o << "SendMaze";
      break;
    case Type::TunePID:
      o << "TunePID";
      break;
    case Type::PlanEnqueue:
      o << "PlanEnqueue";
      break;
    case Type::PlanExecute:
      o << "PlanExecute";
      break;
  }
}

std::ostream &operator<<(std::ostream &o, const PidID id) {
  switch (id) {
    case PidID::Speed:
      o << "Speed";
      break;
    case PidID::Wall:
      o << "Wall";
      break;
    case PidID::Angle:
      o << "Angle";
      break;
  }
}

std::ostream &operator<<(std::ostream &o, const Command *command) {
  o << "remote::command::Command{" << std::endl;
  o << "  type: " << command->type << std::endl;
  o << "  data: {" << std::endl;
  switch (command->type) {
    case Type::Reset:
      break;
    case Type::Explore:
      break;
    case Type::Solve:
      break;
    case Type::SendMaze:
      break;
    case Type::TunePID:
      o << "    pid: {" << std::endl;
      o << "      id: " << command->data.pid.id << std::endl;
      o << "      kp: " << command->data.pid.kp << std::endl;
      o << "      ki: " << command->data.pid.ki << std::endl;
      o << "      kd: " << command->data.pid.kd << std::endl;
      o << "      alpha: " << command->data.pid.alpha << std::endl;
      o << "    }" << std::endl;
      break;
    case Type::PlanEnqueue:
      o << "    plan: " << command->data.plan << std::endl;
      break;
    case Type::PlanExecute:
      break;
    default:
      break;
  }
}
#endif

}  // namespace remote::command
