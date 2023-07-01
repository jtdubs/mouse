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

#define COMMAND_SET_LEDS 0
#define COMMAND_PLAN_POWER 1
#define COMMAND_PLAN_SPEED 2
#define COMMAND_PLAN_LINEAR 3
#define COMMAND_PLAN_EXECUTE 4

#pragma pack(push, 1)
// command_t represents a command that can be processed by the mouse.
typedef struct {
  uint8_t type;
  union {
    struct {
      bool builtin;
      bool left;
      bool right;
      bool ir;
    } leds;
    struct {
      int16_t left;
      int16_t right;
    } power;
    struct {
      float left;
      float right;
    } speed;
    struct {
      float distance;
      float exit_speed;
    } linear;
  } data;
} command_t;
#pragma pack(pop)

// command is the current command.
extern command_t *command;

// command_init initializes the command module.
void command_init();

// command_available determines if a command is available to be processed.
bool command_available();

// command_processed indicates the command has been processed.
void command_processed();
