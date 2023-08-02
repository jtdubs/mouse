#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "usart0_impl.hh"

namespace mouse::platform::usart0 {

namespace {
// The write buffer and associated state.
uint8_t   *write_buffer_;
uint8_t    write_length_;
uint8_t    write_index_;
WriteState write_state_;
uint8_t    write_checksum_;
}  // namespace

// write_ready determines if a write request can be initiated.
bool WriteReady() {
  return write_state_ == WriteState::Idle;
}

// write begins an asynchronous write to USART0.
void Write(uint8_t *buffer, uint8_t length) {
  assert(assert::Module::Usart0Write, 0, buffer != NULL);
  assert(assert::Module::Usart0Write, 1, length > 0);
  assert(assert::Module::Usart0Write, 2, WriteReady());

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // Setup the initial write state.
    write_buffer_   = buffer;
    write_length_   = length;
    write_index_    = 0;
    write_state_    = WriteState::Start;
    write_checksum_ = 0;
  }

  // Initiate the write by enabling the Data Register Empty Interrupt.
  UCSR0B |= 1 << UDRIE0;
}

uint8_t byte;

// The USART0 Data Register Empty Interrupt.
ISR(USART_UDRE_vect, ISR_BLOCK) {
  switch (write_state_) {
    case WriteState::Start:
      // Write the start byte and transition to the next state.
      UDR0         = kStartByte;
      write_state_ = WriteState::Length;
      break;
    case WriteState::Length:
      // Write the length byte and transition to the next state.
      UDR0         = write_length_;
      write_state_ = WriteState::Data;
      break;
    case WriteState::Data:
      // Write the next byte, update the checksum, and transition after all bytes are written.
      byte             = write_buffer_[write_index_++];
      UDR0             = byte;
      write_checksum_ += byte;
      if (write_index_ == write_length_) {
        write_state_ = WriteState::Checksum;
      }
      break;
    case WriteState::Checksum:
      // Write the checksum, disable the interrupt (nothing left to send), and transition to the next state.
      UDR0          = -write_checksum_;
      write_state_  = WriteState::Idle;
      UCSR0B       &= ~_BV(UDRIE0);
      break;
    case WriteState::Idle:
      // Should never happens, as the CHECKSUM state disables the interrupt.
      // But if it does, just turn off the interrupt again...
      UCSR0B &= ~_BV(UDRIE0);
      break;
  }
}

}  // namespace mouse::platform::usart0
