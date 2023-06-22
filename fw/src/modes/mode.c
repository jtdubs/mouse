#include "modes/mode.h"

#include "modes/mode_error.h"
#include "modes/mode_remote.h"
#include "modes/mode_wall.h"
#include "platform/adc.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "serial/command.h"
#include "utils/assert.h"

// The proposed mode (from the DIP switches) and the active mode.
static uint8_t proposed_mode;
static uint8_t active_mode;

// Voltage thresholds for the DIP switches.
// TODO: Should probably pull all of this out into platofrm/mode_selector or something.
static const uint8_t ModeThresholds[16] = {21, 42, 60, 77, 91, 102, 112, 123, 133, 139, 144, 150, 156, 160, 163, 255};

// Register the modes and their mode functions.
#define MODE_COUNT 3
static const mode_t Modes[MODE_COUNT] = {
    [MODE_REMOTE] = {&mode_enter, &mode_remote_tick},     //
    [MODE_WALL]   = {&mode_wall_enter, &mode_wall_tick},  //
    [MODE_ERROR]  = {&mode_enter, &mode_error_tick},      //
};

// mode_init initializes the mode module.
void mode_init() {
  // Enter the initial mode.
  Modes[active_mode].enter();
}

// mode_set sets the active mode.
void mode_set(uint8_t mode) {
  assert(mode < MODE_COUNT);

  if (mode != active_mode) {
    active_mode = mode;
    // Enter the new mode.
    Modes[active_mode].enter();
  }
}

// mode_update handles mode changes initiated through the mode selector or serial command.
void mode_update() {
  // Read the mode selector voltage.
  uint8_t v = mode_selector >> 2;

  // If the button is being pressed, enter the proposed mode.
  if (v > 180) {
    mode_set(proposed_mode);
    return;
  }

  // Otherwise, decode the mode selector voltage.
  for (int i = 0; i < 16; i++) {
    if (v < ModeThresholds[i]) {
      proposed_mode = 15 - i;
      break;
    }
  }

  // If a serial command has been received to set the mode, process it.
  if (command_available() && command.type == COMMAND_SET_MODE) {
    mode_set(command.value);
    command_processed();
  }
}

// mode_tick ticks the active mode.
void mode_tick() {
  Modes[active_mode].tick();
}

// mode_enter is the default enter function.
void mode_enter() {
  // Turn off all LEDs and stop all motors.
  pin_clear(LED_BUILTIN);
  pin_clear(LED_LEFT);
  pin_clear(LED_RIGHT);
  pin_clear(IR_LEDS);
  set_left_motor_dir(true);
  set_left_motor_speed(0);
  set_right_motor_dir(true);
  set_right_motor_speed(0);
}

// mode_get_active() returns the active mode.
uint8_t mode_get_active() {
  return active_mode;
}
