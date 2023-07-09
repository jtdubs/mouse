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

constexpr uint8_t START_BYTE = 0x02;

typedef void (*read_callback_t)(uint8_t* buffer, uint8_t length);

// usart0_init initializes USART0.
void usart0_init();

// usart0_read_ready determines if USART0 is ready to be written to.
bool usart0_write_ready();

// usart0_write begins an asynchronous write to USART0.
void usart0_write(uint8_t* buffer, uint8_t length);

// usart0_disable_receiver disables the USART0 receiver.
void usart0_disable_receiver();

// usart0_enable_receiver enables the USART0 receiver.
void usart0_enable_receiver();

// usart0_set_read_callback sets the read callback for USART0.
void usart0_set_read_callback(read_callback_t callback);
