#pragma once

#include <stdbool.h>
#include <stdint.h>

// Defines the modes.
#define MODE_REMOTE 0
#define MODE_WALL 1
#define MODE_ERROR 2

extern uint8_t mode_active;

// A mode is defined by two functions: enter and tick.
typedef void (*mode_enter_fn)();
typedef void (*mode_tick_fn)();

// mode_t represents a mode.
typedef struct {
  mode_enter_fn enter;
  mode_tick_fn  tick;
} mode_t;

// mode_init initializes the mode module.
void mode_init();

// mode_set sets the active mode.
void mode_set(uint8_t mode);

// mode_update handles mode changes initiated through the mode selector or serial command.
void mode_update();

// mode_tick ticks the active mode.
void mode_tick();

// mode_enter is the default enter function.
void mode_enter();
