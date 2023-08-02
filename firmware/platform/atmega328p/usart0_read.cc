#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "usart0_impl.hh"

namespace mouse::platform::usart0 {

namespace {
// The read buffer and associated state.
uint8_t         read_buffer_[kMaxReadSize];
uint8_t         read_index_;
uint8_t         read_length_;
read_callback_t read_callback_;
ReadState       read_state_;
uint8_t         read_checksum_;
}  // namespace

// DisableReceiver disables the USART0 receiver.
void DisableReceiver() {
  UCSR0B &= ~_BV(RXEN0);
}

// EnableReceiver enables the USART0 receiver.
void EnableReceiver() {
  assert(assert::Module::Usart0Read, 0, read_callback_ != NULL);

  UCSR0B |= _BV(RXEN0);
}

// SetReadCallback sets the read callback for USART0.
void SetReadCallback(read_callback_t callback) {
  assert(assert::Module::Usart0Read, 1, callback != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    read_callback_ = callback;
  }
}

// The USART0 Receive Complete Interrupt.
ISR(USART_RX_vect, ISR_BLOCK) {
  uint8_t value = UDR0;

  switch (read_state_) {
    case ReadState::Idle:
      // If the start byte is received, transition to the next state.
      if (value == kStartByte) {
        read_state_    = ReadState::Length;
        read_checksum_ = 0;
        read_index_    = 0;
      }
      break;
    case ReadState::Length:
      // Validate the length and transition to the next state.
      // Fall back to the idle state if the length is invalid.
      read_length_ = value;
      read_state_  = ReadState::Data;
      if (read_length_ == 0 || read_length_ > kMaxReadSize) {
        read_state_ = ReadState::Idle;
      }
      break;
    case ReadState::Data:
      // Receive the next byte and update the checksum.
      // Transition to the next state after all bytes are received.
      read_buffer_[read_index_++]  = value;
      read_checksum_              += value;
      if (read_index_ == read_length_) {
        read_state_ = ReadState::Checksum;
      }
      break;
    case ReadState::Checksum:
      // Receive the checksum, validate it, and return to the idle state.
      read_checksum_ += value;
      if (read_checksum_ == 0) {
        // If the command is valid:
        // - Disable the receiver (there's nowhere to store the next command).
        // - Invoke the callback.
        DisableReceiver();
        read_callback_(read_buffer_, read_length_);
      }
      read_state_ = ReadState::Idle;
  }
}

}  // namespace mouse::platform::usart0
