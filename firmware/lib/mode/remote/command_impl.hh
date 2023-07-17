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
typedef enum : uint8_t {
  RESET,
  EXPLORE,
  SOLVE,
  SEND_MAZE,
  TUNE_PID,
  PLAN_ENQUEUE,
  PLAN_EXECUTE,
} type_t;

// pid_id_t identifies the PID to tune.
typedef enum : uint8_t {
  PID_SPEED,
  PID_WALL,
  PID_ANGLE,
} pid_id_t;

#pragma pack(push, 1)
// command_t represents a command that can be processed by the mouse.
typedef struct {
  type_t type;
  union {
    struct {
      bool left;
      bool right;
      bool onboard;
      bool ir;
    } leds;
    struct {
      pid_id_t id;
      float    kp;
      float    ki;
      float    kd;
      float    alpha;
    } pid;
    plan::plan_t plan;
  } data;
} command_t;
#pragma pack(pop)

// init initializes the command module.
void init();

// processed indicates the command has been processed.
void processed();

// next gets the next command, if one is available.
bool next(command_t *command);

}  // namespace remote::command
