#pragma once

#include <stdbool.h>
#include <stdint.h>

#define COMMAND_SET_LED 0
#define COMMAND_SET_LEFT_MOTOR_SPEED 1
#define COMMAND_SET_RIGHT_MOTOR_SPEED 2
#define COMMAND_SET_LEFT_MOTOR_DIR 3
#define COMMAND_SET_RIGHT_MOTOR_DIR 4

#pragma pack(push)
#pragma pack(1)
// command_t represents mouse status command.
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
