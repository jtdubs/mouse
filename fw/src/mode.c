#include "mode.h"

#include "adc.h"
#include "command.h"
#include "mode_remote.h"
#include "mode_wall.h"

static uint8_t proposed_mode;
uint8_t        active_mode;

static const uint8_t ModeThresholds[16] = {21, 42, 60, 77, 91, 102, 112, 123, 133, 139, 144, 150, 156, 160, 163, 255};

#define MODE_COUNT 2

static const mode_t Modes[2] = {
    {&mode_remote_enter, &mode_remote_tick},  //
    {&mode_wall_enter, &mode_wall_tick},      //
};

void mode_init() {
  Modes[active_mode].enter();
}

void mode_set(uint8_t mode) {
  if (mode != active_mode && mode < MODE_COUNT) {
    active_mode = mode;
    Modes[active_mode].enter();
  }
}

void mode_update() {
  uint8_t v = mode_selector >> 2;

  if (v > 180) {
    mode_set(proposed_mode);
    return;
  }

  for (int i = 0; i < 16; i++) {
    if (v < ModeThresholds[i]) {
      proposed_mode = 15 - i;
      break;
    }
  }

  if (command_available() && command.type == COMMAND_SET_MODE) {
    mode_set(command.value);
    command_processed();
  }
}

void mode_tick() {
  if (active_mode < MODE_COUNT) {
    Modes[active_mode].tick();
  }
}
