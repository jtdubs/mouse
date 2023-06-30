//
// System: serial
// Module: serial
//
// Purpose:
// - Defines the data structures used to communicate with the host.
// - Provides functions for managing the reports and commands.
//
// Dependencies:
// - Uses the usart0 module to send and receive data.
// - Reads from most platform modules to populate the report.
//
#pragma once

// serial_init initializes the serial module.
void serial_init();
