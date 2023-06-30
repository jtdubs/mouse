#pragma once

#include <stdbool.h>
#include <stdint.h>

#define COMMAND_SET_MODE 0
#define COMMAND_SET_LEDS 1
#define COMMAND_SET_POWER 2
#define COMMAND_SET_SPEED 3
#define COMMAND_SET_POSITION 4
#define COMMAND_SET_SPEED_PID_VARS 5
#define COMMAND_SET_POSITION_PID_VARS 6

#pragma pack(push, 1)
// command_t represents a command that can be processed by the mouse.
typedef struct {
  uint8_t type;
  union {
    uint8_t padding[14];
    struct {
      uint8_t mode;
      uint8_t padding[13];
    } mode;
    struct {
      bool    builtin;
      bool    left;
      bool    right;
      bool    ir;
      uint8_t padding[10];
    } leds;
    struct {
      int16_t left;
      int16_t right;
      uint8_t padding[10];
    } power;
    struct {
      float   left;
      float   right;
      uint8_t padding[6];
    } speed;
    struct {
      float   left;
      float   right;
      uint8_t padding[6];
    } position;
    struct {
      float   kp;
      float   ki;
      float   kd;
      uint8_t padding[2];
    } pid;
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
