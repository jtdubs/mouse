#pragma once

#include <stdbool.h>
#include <stdint.h>

#define COMMAND_SET_MODE 0
#define COMMAND_SET_ONBOARD_LED 1
#define COMMAND_SET_LEFT_LED 2
#define COMMAND_SET_RIGHT_LED 3
#define COMMAND_SET_IR_LEDS 4
#define COMMAND_SET_LEFT_MOTOR_SPEED 5
#define COMMAND_SET_RIGHT_MOTOR_SPEED 6
#define COMMAND_SET_LEFT_MOTOR_FORWARD 7
#define COMMAND_SET_RIGHT_MOTOR_FORWARD 8
#define COMMAND_MAX_VALUE 9

#pragma pack(push, 1)
// command_t represents a command that can be processed by the mouse.
typedef struct {
  uint8_t  type;
  uint16_t value;
} command_t;
#pragma pack(pop)

// command is the current command.
extern command_t command;

// command_init initializes the command module.
void command_init();

// command_available determines if a command is available to be processed.
bool command_available();

// command_processed indicates the command has been processed.
void command_processed();
