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
//     kStartByte | 1            | Always 0x02, used for frame synchronization
//     LENGTH     | 1            | len(DATA)
//     DATA       | LENGTH       |
//     CHECKSUM   | 1            | -sum(DATA), used for error detection
//
#pragma once

#include <stdbool.h>
#include <stdint.h>

namespace mouse::platform::usart0 {

constexpr uint8_t kStartByte   = 0x02;
constexpr uint8_t kMaxReadSize = 64;

// Write states
enum class WriteState : uint8_t {
  Idle,
  Start,
  Length,
  Data,
  Checksum,
};

// Read states
enum class ReadState : uint8_t {
  Idle,
  Length,
  Data,
  Checksum,
};

typedef void (*read_callback_t)(uint8_t *buffer, uint8_t length);

// Init initializes USART0.
void Init();

// write_ready determines if USART0 is ready to be written to.
bool WriteReady();

// write begins an asynchronous write to USART0.
void Write(uint8_t *buffer, uint8_t length);

// DisableReceiver disables the USART0 receiver.
void DisableReceiver();

// EnableReceiver enables the USART0 receiver.
void EnableReceiver();

// SetReadCallback sets the read callback for USART0.
void SetReadCallback(read_callback_t callback);

}  // namespace mouse::platform::usart0
