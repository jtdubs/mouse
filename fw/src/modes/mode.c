#include "modes/mode.h"

#include "control/speed.h"
#include "modes/mode_error.h"
#include "modes/mode_remote.h"
#include "modes/mode_wall.h"
#include "platform/adc.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "platform/selector.h"
#include "serial/command.h"
#include "utils/assert.h"

// The active mode.
uint8_t mode_active;

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
  Modes[mode_active].enter();
}

// mode_set sets the active mode.
void mode_set(uint8_t mode) {
  if (mode != mode_active) {
    assert(ASSERT_MODE + 0, mode < MODE_COUNT);
    mode_active = mode;
    // Enter the new mode.
    Modes[mode_active].enter();
  }
}

// mode_update handles mode changes initiated through the mode selector or serial command.
void mode_update() {
  uint8_t new_mode = selector_update();
  if (new_mode != 0xFF) {
    mode_set(new_mode);
  }

  // If a serial command has been received to set the mode, process it.
  if (command_available() && command.type == COMMAND_SET_MODE) {
    mode_set(command.data.mode.mode);
    command_processed();
  }
}

// mode_tick ticks the active mode.
void mode_tick() {
  Modes[mode_active].tick();
}

// mode_enter is the default enter function.
void mode_enter() {
  // Turn off all LEDs and stop all motors.
  pin_clear(LED_BUILTIN);
  pin_clear(LED_LEFT);
  pin_clear(LED_RIGHT);
  pin_clear(IR_LEDS);
  speed_disable();
  motor_set_forward_left(true);
  motor_set_power_left(0);
  motor_set_forward_right(true);
  motor_set_power_right(0);
}
