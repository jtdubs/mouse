#pragma once

#include <avr/avr_mcu_section.h>
#include <avr/io.h>

// SIM_COMMAND_REG is used to send commands to simavr.
#define SIM_COMMAND_REG GPIOR0

// SIM_CONSOLE_REG is used to send log data to simavr.
#define SIM_CONSOLE_REG GPIOR1

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
