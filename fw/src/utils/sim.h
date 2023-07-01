//
// System: utils
// Module: sim
//
// Purpose:
// - Creates data in the "mmcu" region of the debug binary which is
//   interpreted by simavr to define the simulation parameters.
//
#pragma once

#include <avr/avr_mcu_section.h>
#include <avr/io.h>
#include <stdint.h>

// SIM_COMMAND_REG is used to send commands to simavr.
#define SIM_COMMAND_REG GPIOR0

// SIM_CONSOLE_REG is used to send log data to simavr.
#define SIM_CONSOLE_REG GPIOR1

// SIMAVR_CMD_WATCH_PLAN is a custom command that notifies
// the simulator an interesting plan event is occuring.
#define SIMAVR_CMD_WATCH_PLAN 0x10

inline void sim_watch_plan([[maybe_unused]] uint8_t state) {
#if defined(DEBUG)
  SIM_COMMAND_REG = SIMAVR_CMD_WATCH_PLAN | state;
#endif
}

// sim_start_trace instruction simavr to start the VCD trace.
inline void sim_start_trace() {
#if defined(DEBUG)
  SIM_COMMAND_REG = SIMAVR_CMD_VCD_START_TRACE;
#endif
}

// sim_stop_trace instruction simavr to stop the VCD trace.
inline void sim_stop_trace() {
#if defined(DEBUG)
  SIM_COMMAND_REG = SIMAVR_CMD_VCD_STOP_TRACE;
#endif
}
