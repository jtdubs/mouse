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

typedef enum : uint8_t {
  REPORT_NONE = 0,

  // Individual report types.
  REPORT_PLATFORM = 1,
  REPORT_CONTROL  = 2,
  REPORT_MAZE     = 3,

  REPORT_FIRST = REPORT_PLATFORM,
  REPORT_LAST  = REPORT_MAZE,
} report_type_t;

#pragma pack(push, 1)
// report_header_t represents a mouse report header.
typedef struct {
  report_type_t type;
  uint32_t      rtc_micros;
} report_header_t;

// report_t represents a mouse report.
typedef struct {
  report_header_t header;
  uint8_t         data[59];
} report_t;
#pragma pack(pop)

// report_init initializes the report module.
void report_init();

// report_send sends the next report.
void report_send();
