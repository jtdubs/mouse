//
// System: utils
// Module: sim
//
// Purpose:
// - Creates data in the "mmcu" region of the debug binary which is
//   interpreted by simavr to define the simulation parameters.
// - Define registers that can be used in debug mode to communicate
//   with simavr.
//
#pragma once

#include <avr/avr_mcu_section.h>
#include <avr/io.h>
#include <stdint.h>

// SIM_COMMAND_REG is used to send commands to simavr.
#define SIM_COMMAND_REG GPIOR0

// SIM_CONSOLE_REG is used to send log data to simavr.
#define SIM_CONSOLE_REG GPIOR1

typedef enum : uint8_t {
  // SIMAVR_CMD_WATCH_PLAN is a custom command that notifies
  // the simulator an interesting plan event is occuring.
  SIMAVR_CMD_WATCH_PLAN = 0x10,
} sim_command_t;

inline void sim_watch_plan([[maybe_unused]] uint8_t state) {
#if defined(DEBUG)
  SIM_COMMAND_REG = SIMAVR_CMD_WATCH_PLAN | state;
#endif
}

// start_trace instruction simavr to start the VCD trace.
inline void sim_start_trace() {
#if defined(DEBUG)
  SIM_COMMAND_REG = SIMAVR_CMD_VCD_START_TRACE;
#endif
}

// stop_trace instruction simavr to stop the VCD trace.
inline void sim_stop_trace() {
#if defined(DEBUG)
  SIM_COMMAND_REG = SIMAVR_CMD_VCD_STOP_TRACE;
#endif
}
