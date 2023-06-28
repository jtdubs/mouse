#pragma once

#include <stdbool.h>
#include <stdint.h>

#define COMMAND_SET_MODE 0
#define COMMAND_SET_LEDS 1
#define COMMAND_SET_SPEED 2

#pragma pack(push, 1)
// command_t represents a command that can be processed by the mouse.
typedef struct {
  uint8_t type;
  union {
    uint8_t padding[5];
    struct {
      uint8_t mode;
      uint8_t padding[4];
    } mode;
    struct {
      bool    builtin;
      bool    left;
      bool    right;
      bool    ir;
      uint8_t padding[1];
    } leds;
    struct {
      int16_t left;
      int16_t right;
      uint8_t padding[1];
    } speed;
  } data;
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
