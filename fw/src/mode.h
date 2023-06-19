#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef void (*mode_enter_fn)();
typedef void (*mode_tick_fn)();

typedef struct {
  mode_enter_fn enter;
  mode_tick_fn  tick;
} mode_t;

extern uint8_t active_mode;

void mode_init();
void mode_update();
void mode_tick();
