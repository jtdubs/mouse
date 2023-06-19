#include "mode.h"

#include "adc.h"
#include "command.h"
#include "mode_nop.h"
#include "mode_remote.h"
#include "mode_wall.h"

static uint8_t proposed_mode;
uint8_t        active_mode;

static const uint8_t FunctionSelectThrehsolds[16] = {21,  42,  60,  77,  91,  102, 112, 123,
                                                     133, 139, 144, 150, 156, 160, 163, 255};

static const mode_t Modes[16] = {
    {&mode_remote_enter, &mode_remote_tick},  //
    {&mode_wall_enter, &mode_wall_tick},      //
    {&mode_nop_enter, &mode_nop_tick},        //
    {&mode_nop_enter, &mode_nop_tick},        //
    {&mode_nop_enter, &mode_nop_tick},        //
    {&mode_nop_enter, &mode_nop_tick},        //
    {&mode_nop_enter, &mode_nop_tick},        //
    {&mode_nop_enter, &mode_nop_tick},        //
    {&mode_nop_enter, &mode_nop_tick},        //
    {&mode_nop_enter, &mode_nop_tick},        //
    {&mode_nop_enter, &mode_nop_tick},        //
    {&mode_nop_enter, &mode_nop_tick},        //
    {&mode_nop_enter, &mode_nop_tick},        //
    {&mode_nop_enter, &mode_nop_tick},        //
    {&mode_nop_enter, &mode_nop_tick},        //
    {&mode_nop_enter, &mode_nop_tick},        //
};

void mode_init() {
  proposed_mode = 0;
  active_mode   = 0;
  Modes[active_mode].enter();
}

void mode_set(uint8_t mode) {
  assert(mode < 16);
  if (mode != active_mode) {
    active_mode = mode;
    Modes[active_mode].enter();
  }
}

void mode_update() {
  uint8_t v = (uint8_t)(adc_read(6) >> 2);

  if (v > 180) {
    mode_set(proposed_mode);
    return;
  }

  for (int i = 0; i < 16; i++) {
    if (v < FunctionSelectThrehsolds[i]) {
      proposed_mode = 15 - i;
      break;
    }
  }

  if (command_available() && command.type == COMMAND_SET_MODE) {
    if (command.value < COMMAND_MAX_VALUE) {
      mode_set(command.value);
    }
    command_processed();
  }
}

void mode_tick() {
  Modes[active_mode].tick();
}
