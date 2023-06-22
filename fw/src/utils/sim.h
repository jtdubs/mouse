#pragma once

#include <avr/avr_mcu_section.h>
#include <avr/io.h>

#define SIM_COMMAND_REG GPIOR0
#define SIM_CONSOLE_REG GPIOR1

inline void sim_start_trace() {
#if defined(DEBUG)
  SIM_COMMAND_REG = SIMAVR_CMD_VCD_START_TRACE;
#endif
}

inline void sim_stop_trace() {
#if defined(DEBUG)
  SIM_COMMAND_REG = SIMAVR_CMD_VCD_STOP_TRACE;
#endif
}
