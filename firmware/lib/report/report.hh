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

namespace report {

// Init initializes the report module.
void Init();

}  // namespace report
