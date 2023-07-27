#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "usart0_impl.hh"

namespace usart0 {

namespace {
// The read buffer and associated state.
uint8_t         read_buffer[kMaxReadSize];
uint8_t         read_index;
uint8_t         read_length;
read_callback_t read_callback;
ReadState       read_state;
uint8_t         read_checksum;
}  // namespace

// DisableReceiver disables the USART0 receiver.
void DisableReceiver() {
  // TODO(justindubs): 4809 impl
  UCSR0B &= ~_BV(RXEN0);
}

// EnableReceiver enables the USART0 receiver.
void EnableReceiver() {
  assert(assert::Module::Usart0Read, 0, read_callback != NULL);

  // TODO(justindubs): 4809 impl
  UCSR0B |= _BV(RXEN0);
}

// SetReadCallback sets the read callback for USART0.
void SetReadCallback(read_callback_t callback) {
  assert(assert::Module::Usart0Read, 1, callback != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    read_callback = callback;
  }
}

// The USART0 Receive Complete Interrupt.
// TODO(justindubs): 4809 impl
ISR(USART_RX_vect, ISR_BLOCK) {
  uint8_t value = UDR0;

  switch (read_state) {
    case ReadState::Idle:
      // If the start byte is received, transition to the next state.
      if (value == kStartByte) {
        read_state    = ReadState::Length;
        read_checksum = 0;
        read_index    = 0;
      }
      break;
    case ReadState::Length:
      // Validate the length and transition to the next state.
      // Fall back to the idle state if the length is invalid.
      read_length = value;
      read_state  = ReadState::Data;
      if (read_length == 0 || read_length > kMaxReadSize) {
        read_state = ReadState::Idle;
      }
      break;
    case ReadState::Data:
      // Receive the next byte and update the checksum.
      // Transition to the next state after all bytes are received.
      read_buffer[read_index++]  = value;
      read_checksum             += value;
      if (read_index == read_length) {
        read_state = ReadState::Checksum;
      }
      break;
    case ReadState::Checksum:
      // Receive the checksum, validate it, and return to the idle state.
      read_checksum += value;
      if (read_checksum == 0) {
        // If the command is valid:
        // - Disable the receiver (there's nowhere to store the next command).
        // - Invoke the callback.
        DisableReceiver();
        read_callback(read_buffer, read_length);
      }
      read_state = ReadState::Idle;
  }
}

}  // namespace usart0
