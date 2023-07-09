//
// System: serial
// Module: report
//
// Purpose:
// - Defines the data structure used to send serial reports to the host.
// - Provides functions to trigger building and sending of the report.
// - Communicates with the usart0 module to send the report.
//
// Dependencies:
// - Uses the usart0 module to transmit reports.
// - Reads from most platform modules to populate the report.
//
#pragma once

#include <stdint.h>

#include "control/control.h"
#include "platform/platform.h"

typedef enum : uint8_t {
  REPORT_NONE = 0,

  // Individual report types.
  REPORT_PLATFORM = 1,
  REPORT_CONTROL  = 2,
  // REPORT_MAZE     = 3,

  REPORT_FIRST = REPORT_PLATFORM,
  REPORT_LAST  = REPORT_CONTROL,
} report_type_t;

#pragma pack(push, 1)
// report_t represents a mouse status report.
typedef struct {
  report_type_t type;
  struct {
    uint32_t micros;
  } rtc;
  union {
    platform_report_t platform;
    control_report_t  control;
    // maze_report_t     maze;
  } data;
} report_t;
#pragma pack(pop)

// report_init initializes the report module.
void report_init();

// report_send sends the next report.
void report_send();
