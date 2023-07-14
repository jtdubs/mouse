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
// - Requests reports from the platform, control, and maze modules.
//
// Design:
// - Reports are of variable size, and only transmitted if needed.
// - The modules (maze, control, plaform) are consulted in order to
//   determine if a report should be sent.
// - The first module to say "yes" wins, and the report is sent.
// - The goal is to send the most "important" report, prioritizing
//   updates to the maze state, then the control state, and sending
//   a generic platform report if nothing else is happening.
//
#pragma once

#include <stdint.h>

// report_type_t identifies the type of a given report.
typedef enum : uint8_t {
  // Individual report types.
  REPORT_PLATFORM,
  REPORT_CONTROL,
  REPORT_MAZE,
  REPORT_EXPLORE,
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
  uint8_t         data[48];  // the format of the data is defined by the report type
} report_t;
#pragma pack(pop)

// report_init initializes the report module.
void report_init();

// report_send sends the next report.
void report_send();
