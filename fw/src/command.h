#pragma once

#include <stdbool.h>
#include <stdint.h>

#pragma pack(push)
#pragma pack(1)
// command_t represents mouse status command.
typedef struct {
  uint8_t led;
  uint8_t padding[2];
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
