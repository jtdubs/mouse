//
// System: platform
// Module: usart0
//
// Purpose:
// - Provides functions to send/receive frames the usart0 peripheral.
//
// Peripherals:
// - Owns the USART0 peripheral.
//
// Interrupts:
// - USART_RX when a byte is received.
// - USART_UDRE when a byte can be transmitted.
//
// Protocol:
// - Frames are formatted as follows:
//     Field      | Size (bytes) | Description
//     -----------|--------------|-----------------------------------------------
//     START_BYTE | 1            | Always 0x02, used for frame synchronization
//     LENGTH     | 1            | len(DATA)
//     DATA       | LENGTH       |
//     CHECKSUM   | 1            | -sum(DATA), used for error detection
//
#pragma once

#include <stdbool.h>
#include <stdint.h>

namespace usart0 {

typedef void (*read_callback_t)(uint8_t* buffer, uint8_t length);

// init initializes USART0.
void init();

// write_ready determines if USART0 is ready to be written to.
bool write_ready();

// write begins an asynchronous write to USART0.
void write(uint8_t* buffer, uint8_t length);

// disable_receiver disables the USART0 receiver.
void disable_receiver();

// enable_receiver enables the USART0 receiver.
void enable_receiver();

// set_read_callback sets the read callback for USART0.
void set_read_callback(read_callback_t callback);

}
