#include "mode.h"

#include "adc.h"
#include "command.h"
#include "mode_error.h"
#include "mode_remote.h"
#include "mode_wall.h"
#include "motor.h"
#include "pin.h"

static uint8_t proposed_mode;
uint8_t        active_mode;

static const uint8_t ModeThresholds[16] = {21, 42, 60, 77, 91, 102, 112, 123, 133, 139, 144, 150, 156, 160, 163, 255};

#define MODE_COUNT 3

static const mode_t Modes[MODE_COUNT] = {
    [MODE_REMOTE] = {&mode_enter, &mode_remote_tick},     //
    [MODE_WALL]   = {&mode_wall_enter, &mode_wall_tick},  //
    [MODE_ERROR]  = {&mode_enter, &mode_error_tick},      //
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

void mode_enter() {
  pin_clear(LED_BUILTIN);
  pin_clear(LED_LEFT);
  pin_clear(LED_RIGHT);
  pin_clear(IR_LEDS);
  set_left_motor_dir(true);
  set_left_motor_speed(0);
  set_right_motor_dir(true);
  set_right_motor_speed(0);
}
